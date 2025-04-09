# 🏏 Clover Sports Portal
A simple sports tournament portal with separate Admin and Member logins, built using:
- **Frontend**: HTML, CSS, JavaScript
- **Backend**: C with Mongoose web server

---

## 🔧 Features
- Manual Sign Up / Login
- Admin can upload match & player data
- Members can view match info and leaderboard

---

## 📁 Project Structure
```
Clover-sports/
├── backend/
│   ├── server.c          # C backend with Mongoose
│   ├── users.txt         # User credentials
│   └── match_data.txt    # Match and player data
├── frontend/
│   ├── index.html        # Frontend HTML
│   ├── styles.css        # Styling
│   └── script.js         # JavaScript logic
└── README.md
```

---

## 🚀 How to Run
### 1. Install Mongoose C Library
Download [mongoose.c](https://github.com/cesanta/mongoose/blob/master/mongoose.c) and [mongoose.h](https://github.com/cesanta/mongoose/blob/master/mongoose.h) into the `backend` folder.

### 2. Compile Backend
```bash
cd backend
gcc server.c mongoose.c -o server
```

### 3. Run Server
```bash
./server
```

### 4. Open in Browser
Visit: [http://localhost:8080](http://localhost:8080)

---

## 📝 Default Files
### users.txt
```
admin,adminpass,admin
player1,pass123,member
```

### match_data.txt
```
Match 1: Team A vs Team B - Score: 200/3 vs 198/7
Player of the Match: Player1
```

---

## 💡 Tips
- Add more users manually in `users.txt`
- Edit match/player records in `match_data.txt`

---

## 📬 Questions?
Ping the dev at [your GitHub profile or email]
