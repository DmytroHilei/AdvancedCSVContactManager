// === DOM References ===
const addSection = document.getElementById("add-section");
const showAddBtn = document.getElementById("show-add");
const listBtn = document.getElementById("list-btn");
const hideBtn = document.getElementById("hide-btn");
const listContainer = document.getElementById("contact-list");
const addForm = document.getElementById("add-form");
const freeAllBtn = document.getElementById("freeall-btn");
const sortBtn = document.getElementById("sort-btn");
const searchForm = document.getElementById("search-form");
const searchResult = document.getElementById("search-result");

// === Helper Functions ===
function clearErrors() {
  document.querySelectorAll(".error-note").forEach(el => el.remove());
  document.querySelectorAll("input").forEach(el => {
    el.style.border = "";
    el.title = "";
  });
}

function showError(inputName, message) {
  const field = document.querySelector(`[name="${inputName}"]`);
  if (!field) return;
  field.style.border = "2px solid #ff4d4d";
  field.title = message;
  const note = document.createElement("div");
  note.className = "error-note";
  note.textContent = message;
  field.parentNode.insertBefore(note, field.nextSibling);
}

// === Toggle Add Contact Section ===
showAddBtn.addEventListener("click", () => {
  const visible = addSection.style.display === "block";
  addSection.style.display = visible ? "none" : "block";
  showAddBtn.textContent = visible ? "Add Contact" : "Hide Add Contact";
});

// === Add Contact ===
addForm.addEventListener("submit", async e => {
  e.preventDefault();
  clearErrors();

  const formData = new URLSearchParams(new FormData(addForm));

  try {
    const res = await fetch("/add", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: formData
    });

    const msg = await res.text();

    if (!res.ok) {
      if (msg.includes("name")) showError("name", msg);
      else if (msg.includes("email")) showError("email", msg);
      else if (msg.includes("phone")) showError("phone", msg);
      else if (msg.includes("country")) showError("country", msg);
      else if (msg.includes("city")) showError("city", msg);
      else alert("Error: " + msg);
      return;
    }

    alert(msg);
    addForm.reset();
    addSection.style.display = "none";
    showAddBtn.textContent = "Add Contact";
  } catch (err) {
    console.error("Add contact failed:", err);
    alert("Connection error. Check if the server is running.");
  }
});

// === Show / Hide Contacts ===
listBtn.addEventListener("click", async () => {
  try {
    const res = await fetch("/list");
    const html = await res.text();
    listContainer.innerHTML = html;
    listContainer.style.display = "block";
    hideBtn.style.display = "inline-block";
    listBtn.style.display = "none";
  } catch {
    listContainer.innerHTML = "<p style='color:#ff8080;'>Failed to load contacts.</p>";
  }
});

hideBtn.addEventListener("click", () => {
  listContainer.innerHTML = "";
  listContainer.style.display = "none";
  hideBtn.style.display = "none";
  listBtn.style.display = "inline-block";
});

// === Free All Contacts ===
freeAllBtn.addEventListener("click", async () => {
  if (!confirm("Are you sure you want to delete all contacts?")) return;
  window.location.href = "/freeall";
  try {
    const res = await fetch("/freeall");
    const msg = await res.text();
    listContainer.innerHTML = `<p>${msg}</p>`;
  } catch (err) {
    alert("Failed to contact server: " + err.message);
  }
});

// === Sort Contacts ===
sortBtn.addEventListener("click", async () => {
  try {
    const res = await fetch("/sort");
    const msg = await res.text();
    listContainer.innerHTML = `<p style="color:#80ff80;">${msg}</p>`;

    const listRes = await fetch("/list");
    const html = await listRes.text();
    listContainer.innerHTML += html;
  } catch (err) {
    listContainer.innerHTML = `<p style="color:#ff8080;">Failed to sort contacts: ${err.message}</p>`;
  }
});

// === Search Contact ===
if (searchForm) {
  searchForm.addEventListener("submit", async e => {
    e.preventDefault();
    const formData = new URLSearchParams(new FormData(searchForm));

    try {
      const res = await fetch("/search", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: formData
      });
      const html = await res.text();
      searchResult.innerHTML = html;
    } catch (err) {
      console.error("Search failed:", err);
      searchResult.innerHTML = `<p style='color:#ff8080;'>Search failed: ${err.message}</p>`;
    }
  });
}

// === Handle Delete (Free) via dynamic buttons ===
document.addEventListener("click", async e => {
  if (e.target.classList.contains("delete-btn")) {
    const index = e.target.dataset.index;
    try {
      const res = await fetch("/free-contact", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: `index=${index}`
      });
      await res.text();
      e.target.closest(".contact-card").innerHTML =
          `<p style="color:#2ecc71;font-weight:600;">Contact deleted successfully.</p>`;
    } catch (err) {
      e.target.closest(".contact-card").innerHTML =
          `<p style="color:#e74c3c;">Error deleting contact.</p>`;
    }
  }
});























