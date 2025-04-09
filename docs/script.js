// script.js
function signup() {
  const username = document.getElementById("signup-username").value;
  const password = document.getElementById("signup-password").value;
  const role = document.getElementById("signup-role").value;

  fetch("/signup", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ username, password, role })
  }).then(res => res.json()).then(data => {
    alert(data.message);
  });
}

function login() {
  const username = document.getElementById("login-username").value;
  const password = document.getElementById("login-password").value;

  fetch("/login", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ username, password })
  }).then(res => res.json()).then(data => {
    if (data.success) {
      document.getElementById("login-form").classList.add("hidden");
      document.getElementById("signup-form").classList.add("hidden");

      if (data.role === "admin") {
        document.getElementById("admin-dashboard").classList.remove("hidden");
      } else {
        document.getElementById("member-dashboard").classList.remove("hidden");
        loadMatchInfo();
      }
    } else {
      alert("Login failed: " + data.message);
    }
  });
}

function uploadMatchData() {
  const data = document.getElementById("matchData").value;
  fetch("/upload", {
    method: "POST",
    headers: { "Content-Type": "text/plain" },
    body: data
  }).then(() => {
    alert("Match data uploaded.");
  });
}

function loadMatchInfo() {
  fetch("/matchinfo")
    .then(res => res.text())
    .then(data => {
      document.getElementById("matchInfo").innerText = data;
    });

  fetch("/leaderboard")
    .then(res => res.text())
    .then(data => {
      document.getElementById("leaderboard").innerText = data;
    });
}

function logout() {
  location.reload();
}

