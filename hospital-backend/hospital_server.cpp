/*
 * Hospital Record System - C++ HTTP Backend
 * Integrates with the HTML frontend via REST API on port 8080
 * Uses the same Record2.dat file format as the original C++ program
 *
 * Compile:
 *   Linux/Mac:  g++ -std=c++17 -o hospital_server hospital_server.cpp -lpthread
 *   Windows:    g++ -std=c++17 -o hospital_server.exe hospital_server.cpp -lws2_32
 *
 * Run:  ./hospital_server
 * Then open hospital-management.html in your browser.
 */

#include <httplib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>

// ─── Data file (same as original C++ program) ────────────────────────────────
#define RECORD_FILE "Record2.dat"
#define TEMP_FILE   "temp_record.dat"
#define MAX_NAME    21
#define MAX_ADDR    31
#define MAX_CONTACT 16
#define MAX_EMAIL   31
#define MAX_PROBLEM 21
#define MAX_DOCTOR  31

// ─── Patient struct (mirrors original) ───────────────────────────────────────
struct Patient {
    int  age;
    char gender;
    char fname[MAX_NAME];
    char lname[MAX_NAME];
    char contact[MAX_CONTACT];
    char address[MAX_ADDR];
    char email[MAX_EMAIL];
    char doctor[MAX_DOCTOR];
    char problem[MAX_PROBLEM];
};

// ─── Helpers ─────────────────────────────────────────────────────────────────
void capitalize(char* s) {
    if (s && s[0]) s[0] = toupper((unsigned char)s[0]);
}

bool isAlphaStr(const std::string& s) {
    for (char c : s)
        if (!isalpha((unsigned char)c) && c != ' ') return false;
    return !s.empty();
}

bool isNumericStr(const std::string& s) {
    for (char c : s)
        if (!isdigit((unsigned char)c)) return false;
    return !s.empty();
}

// ─── JSON helpers (no external dependency) ───────────────────────────────────
std::string jsonStr(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        if (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else out += c;
    }
    out += "\"";
    return out;
}

std::string patientToJson(const Patient& p, int idx) {
    std::ostringstream ss;
    ss << "{"
       << "\"id\":"        << idx           << ","
       << "\"fname\":"     << jsonStr(p.fname)   << ","
       << "\"lname\":"     << jsonStr(p.lname)   << ","
       << "\"gender\":"    << jsonStr(std::string(1, p.gender)) << ","
       << "\"age\":"       << p.age              << ","
       << "\"address\":"   << jsonStr(p.address) << ","
       << "\"contact\":"   << jsonStr(p.contact) << ","
       << "\"email\":"     << jsonStr(p.email)   << ","
       << "\"problem\":"   << jsonStr(p.problem) << ","
       << "\"doctor\":"    << jsonStr(p.doctor)  << "}";
    return ss.str();
}

// ─── Simple JSON field extractor ─────────────────────────────────────────────
std::string jsonGet(const std::string& body, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = body.find(search);
    if (pos == std::string::npos) return "";
    pos = body.find(':', pos);
    if (pos == std::string::npos) return "";
    pos++;
    while (pos < body.size() && (body[pos] == ' ' || body[pos] == '\t')) pos++;

    // Number
    if (pos < body.size() && (isdigit(body[pos]) || body[pos] == '-')) {
        size_t end = pos;
        while (end < body.size() && (isdigit(body[end]) || body[end] == '-' || body[end] == '.')) end++;
        return body.substr(pos, end - pos);
    }
    // String
    if (pos < body.size() && body[pos] == '"') {
        pos++;
        std::string val;
        while (pos < body.size() && body[pos] != '"') {
            if (body[pos] == '\\' && pos + 1 < body.size()) {
                pos++;
                if (body[pos] == '"') val += '"';
                else if (body[pos] == 'n') val += '\n';
                else val += body[pos];
            } else {
                val += body[pos];
            }
            pos++;
        }
        return val;
    }
    return "";
}

// ─── File I/O: same format as original program ───────────────────────────────
std::vector<Patient> loadAll() {
    std::vector<Patient> list;
    FILE* f = fopen(RECORD_FILE, "r");
    if (!f) return list;
    Patient p;
    while (fscanf(f, "%20s %20s %c %d %30s %15s %30s %20s %30s\n",
                  p.fname, p.lname, &p.gender, &p.age,
                  p.address, p.contact, p.email, p.problem, p.doctor) == 9) {
        list.push_back(p);
    }
    fclose(f);
    return list;
}

bool saveAll(const std::vector<Patient>& list) {
    FILE* f = fopen(RECORD_FILE, "w");
    if (!f) return false;
    for (const auto& p : list) {
        fprintf(f, "%s %s %c %d %s %s %s %s %s\n",
                p.fname, p.lname, p.gender, p.age,
                p.address, p.contact, p.email, p.problem, p.doctor);
    }
    fclose(f);
    return true;
}

bool appendOne(const Patient& p) {
    FILE* f = fopen(RECORD_FILE, "a");
    if (!f) return false;
    fprintf(f, "%s %s %c %d %s %s %s %s %s\n",
            p.fname, p.lname, p.gender, p.age,
            p.address, p.contact, p.email, p.problem, p.doctor);
    fclose(f);
    return true;
}

// ─── Validation (mirrors original C++ rules) ─────────────────────────────────
std::string validatePatient(const std::string& body, Patient& out) {
    std::string fname   = jsonGet(body, "fname");
    std::string lname   = jsonGet(body, "lname");
    std::string gender  = jsonGet(body, "gender");
    std::string age_s   = jsonGet(body, "age");
    std::string address = jsonGet(body, "address");
    std::string contact = jsonGet(body, "contact");
    std::string email   = jsonGet(body, "email");
    std::string problem = jsonGet(body, "problem");
    std::string doctor  = jsonGet(body, "doctor");

    if (fname.size() < 2 || fname.size() > 20 || !isAlphaStr(fname))
        return "First name must be 2-20 alphabetic characters";
    if (lname.size() < 2 || lname.size() > 20 || !isAlphaStr(lname))
        return "Last name must be 2-20 alphabetic characters";
    if (gender != "M" && gender != "F")
        return "Gender must be M or F";
    int age = atoi(age_s.c_str());
    if (age < 1 || age > 130)
        return "Age must be between 1 and 130";
    if (address.size() < 4 || address.size() > 30)
        return "Address must be 4-30 characters";
    if (contact.size() != 10 || !isNumericStr(contact))
        return "Contact number must be exactly 10 digits";
    if (email.size() < 8 || email.size() > 30 || email.find('@') == std::string::npos)
        return "Email must be 8-30 characters and contain @";
    if (problem.size() < 3 || problem.size() > 20 || !isAlphaStr(problem))
        return "Problem must be 3-20 alphabetic characters";
    if (doctor.size() < 3 || doctor.size() > 30)
        return "Doctor name must be 3-30 characters";

    strncpy(out.fname,   fname.c_str(),   MAX_NAME-1);   capitalize(out.fname);
    strncpy(out.lname,   lname.c_str(),   MAX_NAME-1);   capitalize(out.lname);
    out.gender = gender[0];
    out.age    = age;
    strncpy(out.address, address.c_str(), MAX_ADDR-1);   capitalize(out.address);
    strncpy(out.contact, contact.c_str(), MAX_CONTACT-1);
    strncpy(out.email,   email.c_str(),   MAX_EMAIL-1);
    strncpy(out.problem, problem.c_str(), MAX_PROBLEM-1); capitalize(out.problem);
    strncpy(out.doctor,  doctor.c_str(),  MAX_DOCTOR-1);  capitalize(out.doctor);

    // Null-terminate all
    out.fname[MAX_NAME-1] = out.lname[MAX_NAME-1] = out.address[MAX_ADDR-1] =
    out.contact[MAX_CONTACT-1] = out.email[MAX_EMAIL-1] =
    out.problem[MAX_PROBLEM-1] = out.doctor[MAX_DOCTOR-1] = '\0';

    return ""; // OK
}

// ─── CORS headers helper ──────────────────────────────────────────────────────
void addCors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin",  "*");
    res.set_header("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

void jsonResp(httplib::Response& res, int status, const std::string& body) {
    addCors(res);
    res.status = status;
    res.set_content(body, "application/json");
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    httplib::Server svr;

    // ── OPTIONS (preflight) ──────────────────────────────────────────────────
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        addCors(res);
        res.status = 204;
    });

    // ── GET /api/patients → list all ────────────────────────────────────────
    svr.Get("/api/patients", [](const httplib::Request& req, httplib::Response& res) {
        auto list = loadAll();

        // Optional ?search=name filter
        std::string search = req.has_param("search") ? req.get_param_value("search") : "";
        std::transform(search.begin(), search.end(), search.begin(), ::tolower);
        if (!search.empty() && !search.empty())
            search[0] = toupper(search[0]);

        std::string json = "[";
        bool first = true;
        for (int i = 0; i < (int)list.size(); i++) {
            if (!search.empty() && strcmp(list[i].fname, search.c_str()) != 0) continue;
            if (!first) json += ",";
            json += patientToJson(list[i], i);
            first = false;
        }
        json += "]";
        jsonResp(res, 200, json);
    });

    // ── GET /api/stats ───────────────────────────────────────────────────────
    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        auto list = loadAll();
        int total = list.size(), male = 0, female = 0;
        std::vector<std::string> doctors;
        for (auto& p : list) {
            if (p.gender == 'M') male++;
            else female++;
            std::string d(p.doctor);
            std::transform(d.begin(), d.end(), d.begin(), ::tolower);
            if (std::find(doctors.begin(), doctors.end(), d) == doctors.end())
                doctors.push_back(d);
        }
        std::ostringstream ss;
        ss << "{\"total\":" << total
           << ",\"male\":"  << male
           << ",\"female\":" << female
           << ",\"doctors\":" << doctors.size() << "}";
        jsonResp(res, 200, ss.str());
    });

    // ── POST /api/patients → add ─────────────────────────────────────────────
    svr.Post("/api/patients", [](const httplib::Request& req, httplib::Response& res) {
        Patient p = {};
        std::string err = validatePatient(req.body, p);
        if (!err.empty()) {
            jsonResp(res, 400, "{\"error\":" + jsonStr(err) + "}");
            return;
        }
        if (!appendOne(p)) {
            jsonResp(res, 500, "{\"error\":\"Could not write to file\"}");
            return;
        }
        auto list = loadAll();
        jsonResp(res, 201, patientToJson(list.back(), (int)list.size()-1));
    });

    // ── PUT /api/patients/:id → edit ─────────────────────────────────────────
    svr.Put(R"(/api/patients/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int idx = atoi(req.matches[1].str().c_str());
        auto list = loadAll();
        if (idx < 0 || idx >= (int)list.size()) {
            jsonResp(res, 404, "{\"error\":\"Patient not found\"}");
            return;
        }
        Patient p = {};
        std::string err = validatePatient(req.body, p);
        if (!err.empty()) {
            jsonResp(res, 400, "{\"error\":" + jsonStr(err) + "}");
            return;
        }
        list[idx] = p;
        if (!saveAll(list)) {
            jsonResp(res, 500, "{\"error\":\"Could not write to file\"}");
            return;
        }
        jsonResp(res, 200, patientToJson(list[idx], idx));
    });

    // ── DELETE /api/patients/:id → delete ────────────────────────────────────
    svr.Delete(R"(/api/patients/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int idx = atoi(req.matches[1].str().c_str());
        auto list = loadAll();
        if (idx < 0 || idx >= (int)list.size()) {
            jsonResp(res, 404, "{\"error\":\"Patient not found\"}");
            return;
        }
        list.erase(list.begin() + idx);
        if (!saveAll(list)) {
            jsonResp(res, 500, "{\"error\":\"Could not write to file\"}");
            return;
        }
        jsonResp(res, 200, "{\"message\":\"Patient deleted\"}");
    });

    // ── Health check ─────────────────────────────────────────────────────────
    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        jsonResp(res, 200, "{\"status\":\"ok\",\"server\":\"Hospital Record C++ Backend\"}");
    });

    std::cout << "=========================================\n";
    std::cout << "  Hospital Record System - C++ Backend   \n";
    std::cout << "=========================================\n";
    std::cout << "  Server running at http://localhost:8080 \n";
    std::cout << "  Data file: " << RECORD_FILE << "\n";
    std::cout << "  Press Ctrl+C to stop.\n";
    std::cout << "=========================================\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}
