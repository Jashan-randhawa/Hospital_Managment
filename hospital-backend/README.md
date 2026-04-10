# Hospital Record System — C++ Backend

REST API server built in C++ using [cpp-httplib](https://github.com/yhirose/cpp-httplib).  
Stores patient records in `Record2.dat`.

## Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | /api/health | Server health check |
| GET | /api/patients | List all patients (optional `?search=name`) |
| GET | /api/stats | Dashboard stats |
| POST | /api/patients | Add a patient |
| PUT | /api/patients/:id | Edit a patient |
| DELETE | /api/patients/:id | Delete a patient |

## Deploy on Render

1. Push this repo to GitHub
2. Go to [render.com](https://render.com) → New → Web Service
3. Select this repo
4. Set **Environment** to `Docker`
5. Set **Port** to `8080`
6. Click Deploy

## Run Locally

```bash
# Install httplib header
curl -L https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -o httplib.h

# Compile
g++ -std=c++17 -O2 -o hospital_server hospital_server.cpp -lpthread

# Run
./hospital_server
```

> ⚠️ **Note:** Render's free tier has an ephemeral filesystem.  
> Data in `Record2.dat` will reset on each redeploy/restart.
