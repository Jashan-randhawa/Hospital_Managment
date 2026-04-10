# 🏥 Hospital Record System

A full-stack Hospital Patient Record Management System with a **C++ HTTP backend** and a **modern HTML/CSS/JS frontend**.

🌐 **Live Demo:** [https://hospital-managment-kappa.vercel.app](https://hospital-managment-kappa.vercel.app)

---

## 📸 Overview

This system allows hospital staff to manage patient records through a clean web interface. It supports adding, editing, deleting, and searching patients, with a real-time dashboard showing statistics.

---

## 🏗️ Architecture

```
┌─────────────────────────────┐        ┌──────────────────────────────┐
│        Frontend             │        │         Backend              │
│  HTML + CSS + JavaScript    │◄──────►│   C++ HTTP Server (httplib)  │
│  Hosted on Vercel           │  REST  │   Hosted on Render           │
└─────────────────────────────┘        └──────────────────────────────┘
```

| Layer | Technology | Hosting |
|-------|-----------|---------|
| Frontend | HTML, CSS, Vanilla JS | Vercel |
| Backend | C++17, cpp-httplib | Render (Docker) |
| Storage | Flat file (`Record2.dat`) | Render filesystem |

---

## ✨ Features

- 🔐 Login system with attempt limiting (admin / 12345)
- 📊 Dashboard with live stats (total patients, male/female ratio, doctors count)
- ➕ Add new patient records with full validation
- 📋 View & search all patient records
- ✏️ Edit existing patient records
- 🗑️ Delete patient records
- 🟢 Live server status indicator
- 📱 Responsive design

---

## 🔌 API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/health` | Server health check |
| GET | `/api/patients` | List all patients |
| GET | `/api/patients?search=name` | Search patients by name |
| GET | `/api/stats` | Dashboard statistics |
| POST | `/api/patients` | Add a new patient |
| PUT | `/api/patients/:id` | Update a patient |
| DELETE | `/api/patients/:id` | Delete a patient |

---

## 🚀 Deployment

### Backend → Render
1. Push the `hospital-backend` folder to GitHub
2. Go to [render.com](https://render.com) → **New → Web Service**
3. Set **Environment** to `Docker`, **Port** to `8080`
4. Set **Root Directory** to `hospital-backend`
5. Click **Deploy**

### Frontend → Vercel
1. Update `const API` in `index.html` with your Render URL
2. Push the `hospital-frontend` folder to GitHub
3. Go to [vercel.com](https://vercel.com) → **New Project** → Import repo
4. Click **Deploy**

---

## 💻 Run Locally

### Backend
```bash
# Install httplib header
curl -L https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -o httplib.h

# Compile
g++ -std=c++17 -O2 -o hospital_server hospital_server.cpp -lpthread

# Run
./hospital_server
# Server starts at http://localhost:8080
```

### Frontend
```bash
# Just open in browser — no build step needed
open hospital-frontend/index.html
# Or change API URL to localhost:8080 for local development
```

---

## 📁 Project Structure

```
Hospital_Managment/
├── hospital-backend/
│   ├── hospital_server.cpp   # C++ REST API server
│   ├── Dockerfile            # Docker config for Render
│   ├── Record2.dat           # Patient data file
│   └── README.md
├── hospital-frontend/
│   ├── index.html            # Full frontend (single file)
│   ├── vercel.json           # Vercel routing config
│   └── README.md
└── README.md                 # This file
```

---

## ⚠️ Limitations

- **Data persistence:** Render's free tier has an ephemeral filesystem — data resets on redeploy
- **Cold starts:** Free Render services sleep after 15 min inactivity; first request may take ~30 seconds
- **Auth:** Login is frontend-only (admin/12345) — not production-grade security

---

## 👨‍💻 Author

**Jashanpreet** — [github.com/Jashan-randhawa](https://github.com/Jashan-randhawa)
