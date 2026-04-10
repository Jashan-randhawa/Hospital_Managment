# Hospital Record System — Frontend

Static HTML frontend for the Hospital Record System.  
Communicates with the C++ backend via REST API.

## Setup

1. Deploy the backend first → [hospital-backend repo](../hospital-backend)
2. Copy your Render URL (e.g. `https://hospital-backend-xxxx.onrender.com`)
3. Open `index.html` and replace the API URL on line ~425:
   ```js
   const API = 'https://YOUR-RENDER-APP-NAME.onrender.com/api';
   ```
4. Push this repo to GitHub and deploy on Vercel

## Deploy on Vercel

1. Push this repo to GitHub
2. Go to [vercel.com](https://vercel.com) → New Project
3. Import this repo
4. Click Deploy (no build settings needed — it's a static site)

## Login Credentials

| Username | Password |
|----------|----------|
| admin | 12345 |
