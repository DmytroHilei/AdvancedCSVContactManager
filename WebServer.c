#include "civetweb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
  #include <direct.h>   // _mkdir
#else
  #include <sys/stat.h> // mkdir
#endif

#include "ContactManager.h"   // your functions AddContact, listContact etc.
#include "Validation.h"
#include "WebServer.h"


#include "Sorting.h"
#include "FileIO.h"

#define WEB_ROOT "www"

typedef struct {
    char save_path[256];      // where CivetWeb stores the uploaded file
    char original_name[256];  // client filename (to detect .csv/.txt)
    int  stored;              // flag: file was stored
} UploadCtx;


// ---------------- Global Contact Storage ----------------
extern Contact *contacts;
extern int contactCount;

// ---------------- Utility: Send File ----------------
static void SendFile(struct mg_connection *conn, const char *path, const char *type) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        mg_send_http_error(conn, 404, "File not found");
        return;
    }

    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: %s; charset=utf-8\r\n"
              "\r\n", type);

    char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), f)) > 0)
        mg_write(conn, buffer, n);
    fclose(f);
}

// ---------------- Handlers ----------------

// Serve index.html
static int HandleIndex(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;
    SendFile(conn, WEB_ROOT "/index.html", "text/html");
    return 200;
}

// Serve style.css
static int HandleCSS(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;
    SendFile(conn, WEB_ROOT "/style.css", "text/css");
    return 200;
}

// Serve script.js
static int HandleJS(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;
    SendFile(conn, WEB_ROOT "/script.js", "application/javascript");
    return 200;
}

// Ping handler (for testing connection)
static int HandlePing(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain; charset=utf-8\r\n"
              "\r\n"
              "pong");
    return 200;
}

// Add contact handler
static int HandleAddContact(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    char post_data[1024];
    int len = mg_read(conn, post_data, sizeof(post_data) - 1);
    post_data[len] = '\0';  // null terminate

    char name[100], email[100], phone[100], country[100], city[100];
    mg_get_var(post_data, strlen(post_data), "name", name, sizeof(name));
    mg_get_var(post_data, strlen(post_data), "email", email, sizeof(email));
    mg_get_var(post_data, strlen(post_data), "phone", phone, sizeof(phone));
    mg_get_var(post_data, strlen(post_data), "country", country, sizeof(country));
    mg_get_var(post_data, strlen(post_data), "city", city, sizeof(city));

    if (!IsValidString(name)) {
        mg_printf(conn,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "\r\n"
            "Invalid name or there is duplicate");
        return 400;
    }

    if (!IsValidEmail(email) || IsDuplicateEmail(contacts, &contactCount, email)) {
        mg_printf(conn,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "\r\n"
            "Invalid email address format or there is duplicate");
        return 400;
    }

    if (!IsValidString(country)) {
        mg_printf(conn,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "\r\n"
            "Invalid country name or there is duplicate");
        return 400;
    }

    if (!IsValidPhoneNumber(phone, country) || IsDuplicatePhoneNumber(contacts, &contactCount, phone)) {
        mg_printf(conn,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "\r\n"
            "Invalid phone number for your country or there is duplicate");
        return 400;
    }
    if (!IsValidString(city)) {
        mg_printf(conn,
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "\r\n"
            "Invalid city name or there is duplicate");
        return 400;
    }
    AddContact(contacts, &contactCount, name, email, phone, country, city);

    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain; charset=utf-8\r\n"
              "\r\n"
              "Contact added successfully.");
    return 200;
}

// List contacts handler
static int HandleListContacts(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/html; charset=utf-8\r\n"
              "\r\n");

    mg_printf(conn, "<h3>Contact List</h3>");
    if (contactCount == 0) {
        mg_printf(conn, "<p>No contacts found.</p>");
    } else {
        mg_printf(conn, "<ul>");
        for (int i = 0; i < contactCount; i++) {
            mg_printf(conn,
                "<div class='contact-card'>"
                "<h4>%s</h4>"
                "<p>Email: %s</p>"
                "<p>Phone: %s</p>"
                "<p>Country: %s</p>"
                "<p>City: %s</p>"
                "<div class='btn-group'>"
                  "<form method='POST' action='/free-contact'>"
                    "<input type='hidden' name='index' value='%d'>"
                    "<button type='submit' class='danger'>Free</button>"
                  "</form>"
                  "<form method='POST' action='/edit'>"
                    "<input type='hidden' name='index' value='%d'>"
                    "<button type='submit' class='btn-edit'>Edit</button>"
                  "</form>"
                "</div>"
                "</div>",
                contacts[i].name,
                contacts[i].email,
                contacts[i].phone,
                contacts[i].country,
                contacts[i].city,
                i, i);

        }
        mg_printf(conn, "</ul>");
    }
    return 200;
}



static int HandleFreeAllContacts(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    for (int i = 0; i < contactCount; i++) {
        if (contacts[i].name) free(contacts[i].name);
        if (contacts[i].email) free(contacts[i].email);
        if (contacts[i].phone) free(contacts[i].phone);
        if (contacts[i].country) free(contacts[i].country);
        if (contacts[i].city) free(contacts[i].city);
        memset(&contacts[i], 0, sizeof(Contact));
    }
    contactCount = 0;

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n\r\n"
        "<!DOCTYPE html>"
        "<html lang='en'>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>Database Cleared</title>"
        "<link rel='stylesheet' href='/style.css'>"
        "</head>"
        "<body class='upload-result'>"
        "<div class='card'>"
        "<h1>All contacts freed successfully</h1>"
        "<p>The entire contact database was removed from memory.</p>"
        "<a href='/' class='back-link'>Go back</a>"
        "</div>"
        "</body>"
        "</html>"
    );
    return 200;
}


static int HandleSortingContacts(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "\r\n"
    );

    if (contactCount == 0) {
        mg_printf(conn, "<p>No contacts found.</p>");
        return 200;
    }
    MergeSort(contacts, 0, contactCount - 1);

    mg_printf(conn,
        "<p>Contacts sorted by name successfully.</p>");

    return 200;
}

static int HandleSearchContact(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    char postData[512];
    int len = mg_read(conn, postData, sizeof(postData) - 1);
    postData[len] = '\0';

    char mode[20] = {0}, value[100] = {0};
    mg_get_var(postData, strlen(postData), "mode", mode, sizeof(mode));
    mg_get_var(postData, strlen(postData), "value", value, sizeof(value));

    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "\r\n"
        );

    if (contactCount == 0) {
        mg_printf(conn, "<p>No contacts available.</p>");
        return 200;
    }

    int found = 0;

    for (int i = 0; i < contactCount; i++) {
        int match = 0;

        if (strcmp(mode, "name") == 0 && strcmp(contacts[i].name, value) == 0)
            match = 1;
        else if (strcmp(mode, "email") == 0 && strcmp(contacts[i].email, value) == 0)
            match = 1;
        else if (strcmp(mode, "phone") == 0 && strcmp(contacts[i].phone, value) == 0)
            match = 1;

        if (match) {
            found = 1;
            mg_printf(conn,
                "<div class='contact-card'>"
                  "<h4>%s</h4>"
                  "<p><strong>Email:</strong> %s</p>"
                  "<p><strong>Phone:</strong> %s</p>"
                  "<p><strong>Country:</strong> %s</p>"
                  "<p><strong>City:</strong> %s</p>"
                  "<div class='btn-group'>"
                    "<form method='POST' action='/free-contact' style='display:inline;'>"
                      "<input type='hidden' name='index' value='%d'>"
                      "<button type='submit' class='danger'>Free</button>"
                    "</form>"
                    "<form method='POST' action='/edit' style='display:inline;'>"
                      "<input type='hidden' name='index' value='%d'>"
                      "<button type='submit' class='btn-edit'>Edit</button>"
                    "</form>"
                  "</div>"
                "</div>",
                contacts[i].name,
                contacts[i].email,
                contacts[i].phone,
                contacts[i].country,
                contacts[i].city,
                i, i
            );

            }
        }
    if (!found) {
        mg_printf(conn, "<p>No contacts found for '%s'.</p>", value);
    }
    mg_printf(conn,
    "<br><a href='/' class='back-link'>Go back</a>"
    "</div></body></html>"
);

    return 200;
}

static int HandleFreeContact(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    char postData[64];
    int len = mg_read(conn, postData, sizeof(postData) - 1);
    if (len < 0) len = 0;
    postData[len] = '\0';

    char idxStr[10] = {0};
    mg_get_var(postData, len, "index", idxStr, sizeof(idxStr));
    int index = atoi(idxStr);

    if (index < 0 || index >= contactCount) {
        mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n\r\n"
            "<!DOCTYPE html>"
            "<html><body class='upload-result'>"
            "<div class='card'>"
            "<h1 style='color:#ff5555;'>Invalid index</h1>"
            "<p>No contact found at this position.</p>"
            "<a href='/' class='back-link'>Go back</a>"
            "</div></body></html>"
        );
        return 200;
    }

    free(contacts[index].name);
    free(contacts[index].email);
    free(contacts[index].phone);
    free(contacts[index].country);
    free(contacts[index].city);

    // Shift elements left
    for (int j = index; j < contactCount - 1; j++)
        contacts[j] = contacts[j + 1];
    memset(&contacts[contactCount - 1], 0, sizeof(Contact));
    contactCount--;

    // ✅ single proper response
    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n\r\n"
        "<!DOCTYPE html>"
        "<html lang='en'>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>Contact freed</title>"
        "<link rel='stylesheet' href='/style.css'>"
        "</head>"
        "<body class='upload-result'>"
        "<div class='card'>"
        "<h1 style='color:#00ff55;'>Contact freed successfully</h1>"
        "<p>The contact record was removed from memory.</p>"
        "<a href='/' class='back-link'>Go back</a>"
        "</div>"
        "</body>"
        "</html>"
    );

    return 200;
}


static int HandleEditContact(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    char postData[128];
    int len = mg_read(conn, postData, sizeof(postData) - 1);
    if (len < 0) len = 0;
    postData[len] = '\0';

    char idxStr[10] = {0};
    mg_get_var(postData, len, "index", idxStr, sizeof(idxStr));
    int index;
    index = atoi(idxStr);

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "\r\n"
        "<!DOCTYPE html><html lang='en'><head>"
        "<meta charset='UTF-8'>"
        "<title>Edit Contact</title>"
        "<link rel='stylesheet' href='/style.css'>"
        "</head><body>"
    );

    if (index < 0 || index >= contactCount) {
        mg_printf(conn, "<p>Invalid index.</p>");
        return 200;
    }

    mg_printf(conn,
    "<div class='container'>"
    "<h2>Edit Contact: %s</h2>"
    "<form method='POST' action='/apply-edit'>"
    "<input type='hidden' name='index' value='%d'>"
    "<label for='field'>Select field to edit:</label><br>"
    "<select name='field' required>"
    "<option value='name'>Name</option>"
    "<option value='email'>Email</option>"
    "<option value='phone'>Phone</option>"
    "<option value='country'>Country</option>"
    "<option value='city'>City</option>"
    "</select><br><br>"
    "<input type='text' name='value' placeholder='Enter new value...' required><br>"
    "<button type='submit'>Apply Edit</button>"
    "</form>"
    "<a href='/' style='color:#00aaff;'>Cancel</a>"
    "</div>"
    "</body></html>",
    contacts[index].name, index
);

}

static int HandleApplyEdit(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    char postData[512];
    int len = mg_read(conn, postData, sizeof(postData) - 1);
    if (len < 0) len = 0;
    postData[len] = '\0';

    char idxStr[10] = {0}, field[50] = {0}, value[200] = {0};
    mg_get_var(postData, len, "index", idxStr, sizeof(idxStr));
    mg_get_var(postData, len, "field", field, sizeof(field));
    mg_get_var(postData, len, "value", value, sizeof(value));

    int index = atoi(idxStr);

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "\r\n"
        "<!DOCTYPE html><html lang='en'><head>"
        "<meta charset='UTF-8'>"
        "<title>Edit Result</title>"
        "<link rel='stylesheet' href='/style.css'>"
        "</head><body><div class='container'>"
    );

    if (index < 0 || index >= contactCount) {
        mg_printf(conn, "<p>Invalid contact index.</p></div></body></html>");
        return 200;
    }

    // free and replace based on field

    if (strcmp(field, "name") == 0) {
            char *new_name = strdup(value);
            if (!new_name) {
                mg_printf(conn,
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Memory allocation failed");
                return 500;
            }
            free(contacts[index].name);
            contacts[index].name = new_name;
        } else if (strcmp(field, "email") == 0) {
            char *newEmail = strdup(value);
            if (!newEmail) {
                mg_printf(conn,
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Memory allocation failed");
                return 500;
            }
            if (IsDuplicateEmail(contacts, &contactCount, newEmail)) {
                mg_printf(conn,
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: text/html; charset=utf-8\r\n"
                    "\r\n"
                    "Invalid name or there is duplicate");
                return 400;
            }
            free(contacts[index].email);
            contacts[index].email = newEmail;
        } else if (strcmp(field, "phone") == 0) {
            char *newPhone = strdup(value);
            if (!newPhone) {
                mg_printf(conn,
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Memory allocation failed");
                return 500;
            }
            if (IsDuplicatePhoneNumber(contacts, &contactCount, newPhone)) {
                mg_printf(conn,
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: text/html; charset=utf-8\r\n"
                    "\r\n"
                    "Invalid name or there is duplicate");
                return 400;
            }
            free(contacts[index].phone);
            contacts[index].phone = newPhone;
        } else if (strcmp(field, "country") == 0) {
            char *newCountry = strdup(value);
            if (!newCountry) {
                mg_printf(conn,
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Memory allocation failed");
            }
            free(contacts[index].country);
            contacts[index].country = newCountry;
        } else if (strcmp(field, "city") == 0) {
            char *newCity = strdup(value);
            if (!newCity) {
                mg_printf(conn,
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Memory allocation failed");
            }
            free(contacts[index].city);
            contacts[index].city = newCity;
        }

    mg_printf(conn,
        "<p>Contact updated successfully.</p>"
        "<a href='/' style='color:#00aaff;'>Return to main page</a>"
        "</div></body></html>"
    );

    return 200;
}

static int on_field_found(const char *key,
                          const char *filename,
                          char *path,
                          size_t pathlen,
                          void *user_data)
{
    UploadCtx *ctx = (UploadCtx *)user_data;

    // We only care about the file field. Typical name: "file".
    if (filename && *filename) {
        // Remember original client filename (for extension detection).
        snprintf(ctx->original_name, sizeof(ctx->original_name), "%s", filename);

        // Tell CivetWeb where to store the uploaded file.
        // Use a unique temp name under uploads/
        snprintf(ctx->save_path, sizeof(ctx->save_path),
                 "uploads/uploaded_%ld.tmp", (long)time(NULL));
        snprintf(path, pathlen, "%s", ctx->save_path);

        return MG_FORM_FIELD_STORAGE_STORE;
    }

    // For non-file fields, we don’t need them here.
    (void)key;
    return MG_FORM_FIELD_STORAGE_SKIP;
}

static int on_field_get(const char *key,
                        const char *value,
                        size_t valuelen,
                        void *user_data) {
    (void)key; (void)value; (void)valuelen; (void)user_data;
    // Continue to next field
    return MG_FORM_FIELD_HANDLE_NEXT;
}

static int on_field_store(const char *path,
                          long long file_size,
                          void *user_data)
{
    UploadCtx *ctx = (UploadCtx *)user_data;
    // Sanity: ensure it's the file we expected
    if (strcmp(path, ctx->save_path) == 0 && file_size > 0) {
        ctx->stored = 1;
    }
    return MG_FORM_FIELD_HANDLE_NEXT;
}


static int HandleUploadFile(struct mg_connection *conn, void *cbdata)
{
    (void)cbdata;

    // Ensure uploads directory exists
#ifdef _WIN32
    _mkdir("uploads");
#else
    mkdir("uploads", 0777);
#endif

    UploadCtx ctx;
    memset(&ctx, 0, sizeof(ctx));

    struct mg_form_data_handler fdh;
    memset(&fdh, 0, sizeof(fdh));
    fdh.field_found = on_field_found;
    fdh.field_get   = on_field_get;
    fdh.field_store = on_field_store;
    fdh.user_data   = &ctx;

    // Parse the multipart/form-data and write file
    int handled = mg_handle_form_request(conn, &fdh);
    // handled >= 0 means number of fields processed; < 0 means error

    int added = 0;
    if (handled >= 0 && ctx.stored) {
        // Determine format by ORIGINAL client filename (not the .tmp)
        const char *ext = strrchr(ctx.original_name, '.');
        if (ext && _stricmp(ext, ".csv") == 0) {
            added = ReadFromTheCSV(ctx.save_path, &contacts, &contactCount);
        } else {
            // Default to TXT if unknown
            added = ReadFromTXT(ctx.save_path, &contacts, &contactCount);
        }
        // Remove the temp file after parsing
        remove(ctx.save_path);
    }

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n\r\n"
        "<!DOCTYPE html>"
        "<html lang='en'>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>Upload result</title>"
        "<link rel='stylesheet' href='/style.css'>"
        "</head>"
        "<body class='upload-result'>"        // ✅ use correct class
        "<div class='card'>"                  // ✅ use correct class
        "<h1>Upload %s</h1>"                  // no need for old status-title
        "<p>%s</p>"
        "<p>%d new contacts imported.</p>"
        "<a href='/' class='back-link'>Go back</a>"
        "</div>"
        "</body>"
        "</html>",
        (handled < 0) ? "failed" : "successful",
        (ctx.stored ? "File saved and parsed." : "No file stored."),
        added
    );



    return 200;
}

static int HandleDownloadTXT(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Disposition: attachment; filename=\"contacts.txt\"\r\n\r\n"
    );

    for (int i = 0; i < contactCount; i++) {
        mg_printf(conn,
            "Name: %s\nEmail: %s\nPhone: %s\nCountry: %s\nCity: %s\n\n",
            contacts[i].name,
            contacts[i].email,
            contacts[i].phone,
            contacts[i].country,
            contacts[i].city
        );
    }

    return 200;
}

static int HandleDownloadCSV(struct mg_connection *conn, void *cbdata) {
    (void)cbdata;

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/csv; charset=utf-8\r\n"
        "Content-Disposition: attachment; filename=\"contacts.csv\"\r\n\r\n"
    );

    // Header row
    mg_printf(conn, "Name,Email,Phone,Country,City\n");

    for (int i = 0; i < contactCount; i++) {
        mg_printf(conn, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
            contacts[i].name,
            contacts[i].email,
            contacts[i].phone,
            contacts[i].country,
            contacts[i].city
        );
    }

    return 200;
}


// ---------------- Start Server ----------------
void StartWebServer() {
    struct mg_context *ctx;

    const char *options[] = {
        "listening_ports", "8080",
        "document_root", WEB_ROOT,
        NULL
    };

    printf("[INFO] Launching web server...\n");

    ctx = mg_start(NULL, 0, options);

    if (!ctx) {
        printf("[ERROR] Failed to start CivetWeb server\n");
        return;
    }

    mg_set_request_handler(ctx, "/", HandleIndex, NULL);
    mg_set_request_handler(ctx, "/style.css", HandleCSS, NULL);
    mg_set_request_handler(ctx, "/script.js", HandleJS, NULL);
    mg_set_request_handler(ctx, "/api/ping", HandlePing, NULL);
    mg_set_request_handler(ctx, "/add", HandleAddContact, NULL);
    mg_set_request_handler(ctx, "/list", HandleListContacts, NULL);
    mg_set_request_handler(ctx, "/freeall", HandleFreeAllContacts, NULL);
    mg_set_request_handler(ctx, "/search", HandleSearchContact, NULL);
    mg_set_request_handler(ctx, "/sort", HandleSortingContacts, NULL);
    mg_set_request_handler(ctx, "/free-contact", HandleFreeContact, NULL);
    mg_set_request_handler(ctx, "/edit", HandleEditContact, NULL);
    mg_set_request_handler(ctx, "/apply-edit", HandleApplyEdit, NULL);
    mg_set_request_handler(ctx, "/upload", HandleUploadFile, NULL);
    mg_set_request_handler(ctx, "/download-txt", HandleDownloadTXT, NULL);
    mg_set_request_handler(ctx, "/download-csv", HandleDownloadCSV, NULL);

    printf("[OK] Server running at: http://localhost:8080/\n");
    printf("[OK] Press Enter to stop.\n");
    getchar();

    mg_stop(ctx);
    printf("[INFO] Server stopped.\n");
}