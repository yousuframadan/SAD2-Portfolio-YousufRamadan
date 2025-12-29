#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
using namespace std;

// Forward declarations (unchanged)
class Patient;
class Doctor;
class Prescription;
class MedicalRecord;
class Appointment;
class HospitalSystem;
class Admin;

// ----------------- User (abstract) -----------------
class User {
public:
    int id;
    string name;
    bool isActive;
    User(int i = 0, const string& n = "") : id(i), name(n), isActive(true) {}
    virtual void login() = 0;
    virtual void logout() = 0;
    virtual ~User() {}
};

// ----------------- Prescription -----------------
class Prescription {
public:
    int id;
    string medicine;
    string dosage;
    Doctor* doctor;
    Patient* patient;
    Prescription(int i, const string& med, const string& d, Doctor* doc, Patient* pat)
        : id(i), medicine(med), dosage(d), doctor(doc), patient(pat) {
    }
    void show();
};

// ----------------- MedicalRecord -----------------
class MedicalRecord {
public:
    int id;
    string history;
    Prescription* prescription;
    MedicalRecord(int i, const string& h, Prescription* p) : id(i), history(h), prescription(p) {}
    void show();
};

// ----------------- Appointment -----------------
class Appointment {
public:
    int id;
    string date, time;
    Patient* patient;
    Doctor* doctor;
    Appointment(int i, const string& d, const string& t, Patient* p, Doctor* doc)
        : id(i), date(d), time(t), patient(p), doctor(doc) {
    }
    void show();
};

class TimeSlotProvider {
public:
    virtual vector<string> getSlots() const {
        return { "09:00","10:00","11:00","12:00","13:00","14:00","15:00" };
    }
    virtual ~TimeSlotProvider() {}
};

// ----------------- PrescriptionService (SRP Applied) -----------------

class PrescriptionService {
public:

    Prescription* createPrescription(
        Doctor* doctor,
        Patient* patient,
        int presId,
        int recordId,
        const string& med,
        const string& dose,
        const string& history,
        MedicalRecord*& outRecord
    );
    virtual ~PrescriptionService() {}
};

// ----------------- Patient -----------------
class Patient : public User {
public:
    MedicalRecord* record = nullptr;
    Appointment* appointment = nullptr;

    Patient(int i = 0, const string& n = "") : User(i, n) {}

    void login() override;
    void logout() override;

    bool hasConflict(const vector<Appointment*>& all, const string& date, const string& time) const;
    Appointment* makeAppointment(int appId, const string& date, const string& time, Doctor* d);

    void viewRecord();
};

// ----------------- Doctor -----------------
// Modified: Doctor no longer creates prescriptions directly.
// It holds a pointer to PrescriptionService (injected by HospitalSystem).
class Doctor : public User {
public:
    string specialization;

    // SRP: Doctor delegates prescription creation to PrescriptionService.
    PrescriptionService* presService = nullptr;

    Doctor(int i = 0, const string& n = "", const string& s = "") : User(i, n), specialization(s) {}

    void login() override;
    void logout() override;

    vector<Appointment*> myAppointments(const vector<Appointment*>& all) const;

    // New method: writePrescription -> calls the PrescriptionService
    Prescription* writePrescription(
        Patient* p,
        int presId,
        int recordId,
        const string& med,
        const string& dose,
        const string& history,
        MedicalRecord*& outRecord
    );

    MedicalRecord* getRecord(Patient* p) const;
};

// ----------------- Admin -----------------
class Admin : public User {
public:
    Admin(int i = 0, const string& n = "") : User(i, n) {}
    void login() override;
    void logout() override;

    void disablePatient(HospitalSystem& sys, int patientId);
    void disableDoctor(HospitalSystem& sys, int doctorId);
    void showAllPatients(HospitalSystem& sys);
    void showAllDoctors(HospitalSystem& sys);
    void addDoctor(HospitalSystem& sys); // NEW
};

// ----------------- HospitalSystem -----------------
// Modified: uses TimeSlotProvider (OCP) and PrescriptionService (SRP).
class HospitalSystem {
private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Appointment*> appointments;
    vector<Prescription*> prescriptions;
    vector<MedicalRecord*> records;

   ;

    int nextAppt = 1;
    int nextPres = 1;
    int nextRec = 1;

    // New: providers/services
    TimeSlotProvider* slotProvider = nullptr;      // OCP
    PrescriptionService* presService = nullptr;    // SRP

public:
    HospitalSystem();
    ~HospitalSystem();

    // repository helpers
    Patient* findPatient(int id);
    Doctor* findDoctor(int id);
    bool idTaken(int id);
    bool slotFree(Doctor* d, const string& date, const string& time);
    void storePrescription(Prescription* p);
    void storeMedicalRecord(MedicalRecord* r);
    bool storeAppointment(Appointment* a);

    // NEW: add doctor
    void addDoctor(int id, const string& name, const string& spec);

    // initial data
    void initDoctors();

    // UI
    void run();
    void adminMenu(Admin& adminUser);
    void doctorLogin();
    void patientPortal();
    void patientMenu(Patient* p);
    void bookAppointmentFor(Patient* p);

    // helpers
    string getDate(int offset);

    // helpers for admin listing
    vector<Patient*> allActivePatients();
    vector<Doctor*> allActiveDoctors();
};

// ----------------- Implementations -----------------

// Prescription
void Prescription::show() {
    cout << "Prescription#" << id << ": " << medicine << " (" << dosage << ") for "
        << (patient ? patient->name : string("Unknown")) << " by "
        << (doctor ? doctor->name : string("Unknown")) << "\n";
}

// MedicalRecord
void MedicalRecord::show() {
    cout << "Record#" << id << " | History: " << history << "\n";
    if (prescription) prescription->show();
}

// Appointment
void Appointment::show() {
    cout << "Appt#" << id << " | " << date << " " << time
        << " | Patient: " << (patient ? patient->name : string("Unknown"))
        << " | Doctor: " << (doctor ? doctor->name : string("Unknown"));
    if (doctor) cout << " (" << doctor->specialization << ")";
    cout << "\n";
}

// ----------------- PrescriptionService implementation -----------------
Prescription* PrescriptionService::createPrescription(
    Doctor* doctor,
    Patient* patient,
    int presId,
    int recordId,
    const string& med,
    const string& dose,
    const string& history,
    MedicalRecord*& outRecord
) {
    // Create the Prescription object
    Prescription* pres = new Prescription(presId, med, dose, doctor, patient);

    // Manage the patient's medical record (create or update)
    if (!patient->record) {
        MedicalRecord* rec = new MedicalRecord(recordId, history, pres);
        patient->record = rec;
        outRecord = rec;
    }
    else {
        patient->record->history = history;
        patient->record->prescription = pres;
        outRecord = nullptr;
    }

    return pres;
}

// ----------------- Patient -----------------
void Patient::login() { cout << name << " (Patient) logged in.\n"; }
void Patient::logout() { cout << name << " logged out.\n"; }

bool Patient::hasConflict(const vector<Appointment*>& all, const string& date, const string& time) const {
    for (auto a : all) if (a->patient == this && a->date == date && a->time == time) return true;
    return false;
}
Appointment* Patient::makeAppointment(int appId, const string& date, const string& time, Doctor* d) {
    Appointment* a = new Appointment(appId, date, time, this, d);
    this->appointment = a;
    return a;
}
void Patient::viewRecord() {
    if (!record) { cout << "No medical record for " << name << ".\n"; return; }
    record->show();
}

// ----------------- Doctor -----------------
void Doctor::login() { cout << name << " (Doctor) logged in.\n"; }
void Doctor::logout() { cout << name << " logged out.\n"; }

vector<Appointment*> Doctor::myAppointments(const vector<Appointment*>& all) const {
    vector<Appointment*> out;
    for (auto a : all) if (a->doctor == this) out.push_back(a);
    return out;
}

// Doctor delegates to PrescriptionService (SRP)
Prescription* Doctor::writePrescription(Patient* p, int presId, int recordId,
    const string& med, const string& dose, const string& history,
    MedicalRecord*& outRecord) {
    if (!presService) {
        cout << "[ERROR] PrescriptionService is not set for Doctor " << name << "!\n";
        return nullptr;
    }
    return presService->createPrescription(this, p, presId, recordId, med, dose, history, outRecord);
}

MedicalRecord* Doctor::getRecord(Patient* p) const {
    return p->record;
}

// ----------------- Admin -----------------
void Admin::login() { cout << name << " (Admin) logged in.\n"; }
void Admin::logout() { cout << name << " logged out.\n"; }

void Admin::disablePatient(HospitalSystem& sys, int patientId) {
    Patient* p = sys.findPatient(patientId);
    if (!p) { cout << "Patient not found\n"; return; }
    p->isActive = false; cout << "Disabled patient " << p->name << "\n";
}
void Admin::disableDoctor(HospitalSystem& sys, int doctorId) {
    Doctor* d = sys.findDoctor(doctorId);
    if (!d) { cout << "Doctor not found\n"; return; }
    d->isActive = false; cout << "Disabled doctor " << d->name << "\n";
}
void Admin::showAllPatients(HospitalSystem& sys) {
    auto list = sys.allActivePatients();
    cout << "--- Patients ---\n";
    for (auto p : list) cout << p->id << " | " << p->name << "\n";
}
void Admin::showAllDoctors(HospitalSystem& sys) {
    auto list = sys.allActiveDoctors();
    cout << "--- Doctors ---\n";
    for (auto d : list) cout << d->id << " | " << d->name << " | " << d->specialization << "\n";
}
void Admin::addDoctor(HospitalSystem& sys) {
    int id; string name, spec;
    cout << "Enter new Doctor ID: ";
    cin >> id; cin.ignore();
    if (sys.idTaken(id)) {
        cout << "This ID is already used!\n";
        return;
    }
    cout << "Enter Doctor Name: ";
    getline(cin, name);
    cout << "Enter Doctor Specialization: ";
    getline(cin, spec);

    sys.addDoctor(id, name, spec);
    cout << "Doctor added successfully.\n";
}

// ----------------- HospitalSystem (impl) -----------------
HospitalSystem::HospitalSystem() {
    // Initialize providers/services (new)
    slotProvider = new TimeSlotProvider();      // default provider (can be extended later)
    presService = new PrescriptionService();    // prescription logic separated

    initDoctors();

    // inject PrescriptionService into existing Doctor objects (SRP)
    for (auto& d : doctors) {
        d.presService = presService;
    }
}
HospitalSystem::~HospitalSystem() {
    for (auto a : appointments) delete a;
    for (auto p : prescriptions) delete p;
    for (auto r : records) delete r;

    // delete services/providers
    delete slotProvider;
    delete presService;
}

Patient* HospitalSystem::findPatient(int id) {
    for (auto& p : patients) if (p.id == id && p.isActive) return &p;
    return nullptr;
}
Doctor* HospitalSystem::findDoctor(int id) {
    for (auto& d : doctors) if (d.id == id && d.isActive) return &d;
    return nullptr;
}
bool HospitalSystem::idTaken(int id) {
    for (auto& p : patients) if (p.id == id) return true;
    for (auto& d : doctors) if (d.id == id) return true;
    return false;
}
bool HospitalSystem::slotFree(Doctor* d, const string& date, const string& time) {
    for (auto a : appointments) if (a->doctor == d && a->date == date && a->time == time) return false;
    return true;
}
void HospitalSystem::storePrescription(Prescription* p) { prescriptions.push_back(p); }
void HospitalSystem::storeMedicalRecord(MedicalRecord* r) { records.push_back(r); }
bool HospitalSystem::storeAppointment(Appointment* a) {
    if (!slotFree(a->doctor, a->date, a->time)) return false;
    appointments.push_back(a);
    return true;
}

// NEW: addDoctor implementation
void HospitalSystem::addDoctor(int id, const string& name, const string& spec) {
    doctors.push_back(Doctor(id, name, spec));
    // Inject the presService for the new doctor (maintain SRP contract)
    doctors.back().presService = presService;
}

void HospitalSystem::initDoctors() {
    doctors.push_back(Doctor(1001, "Dr. Ahmed", "Cardiology"));
    doctors.push_back(Doctor(1002, "Dr. Sara", "Pediatrics"));
    doctors.push_back(Doctor(1003, "Dr. Omar", "Orthopedics"));
}

void HospitalSystem::run() {
    int choice; Admin admin(9000, "Admin");
    bool running = true;
    while (running) {
        cout << "\n--- Hospital ---\n1 Admin\n2 Doctor\n3 Patient\n0 Exit\nChoose: ";
        cin >> choice; cin.ignore();
        switch (choice) {
        case 1: adminMenu(admin); break;
        case 2: doctorLogin(); break;
        case 3: patientPortal(); break;
        case 0: running = false; break;
        default: cout << "Invalid\n";
        }
    }
    cout << "Goodbye\n";
}

void HospitalSystem::adminMenu(Admin& adminUser) {
    adminUser.login();
    bool inAdmin = true;
    while (inAdmin) {
        cout << "\n--- Admin Menu ---\n"
            << "1 Disable Patient\n"
            << "2 Disable Doctor\n"
            << "3 Show Patients\n"
            << "4 Show Doctors\n"
            << "5 Add Doctor\n"
            << "0 Logout\nChoose: ";
        int c; cin >> c; cin.ignore();
        switch (c) {
        case 1: { int id; cout << "Patient ID: "; cin >> id; cin.ignore(); adminUser.disablePatient(*this, id); break; }
        case 2: { int id; cout << "Doctor ID: "; cin >> id; cin.ignore(); adminUser.disableDoctor(*this, id); break; }
        case 3: adminUser.showAllPatients(*this); break;
        case 4: adminUser.showAllDoctors(*this); break;
        case 5: adminUser.addDoctor(*this); break;
        case 0: inAdmin = false; break;
        default: cout << "Invalid\n";
        }
    }
    adminUser.logout();
}

void HospitalSystem::doctorLogin() {
    int id; cout << "Enter Doctor ID: "; cin >> id; cin.ignore();
    Doctor* d = findDoctor(id);
    if (!d) { cout << "Doctor not found or disabled.\n"; return; }
    d->login();
    bool inDoc = true;
    while (inDoc) {
        cout << "\n--- Doctor Menu (" << d->name << ") ---\n1 View My Appointments\n2 Write Prescription\n0 Logout\nChoose: ";
        int c; cin >> c; cin.ignore();
        if (c == 1) {
            auto list = d->myAppointments(appointments);
            if (list.empty()) cout << "No appointments\n";
            else for (auto a : list) a->show();
        }
        else if (c == 2) {
            int pid; cout << "Patient ID: "; cin >> pid; cin.ignore();
            Patient* p = findPatient(pid);
            if (!p) { cout << "Patient not found.\n"; continue; }
            string med, dose, hist;
            cout << "Medicine: "; getline(cin, med);
            cout << "Dosage: "; getline(cin, dose);
            cout << "History: "; getline(cin, hist);
            MedicalRecord* newRec = nullptr;

            // NOTE: Doctor now delegates to PrescriptionService (SRP)
            Prescription* pres = d->writePrescription(p, nextPres++, nextRec, med, dose, hist, newRec);
            if (pres) {
                storePrescription(pres);
                if (newRec) { storeMedicalRecord(newRec); nextRec++; }
                cout << "Prescription created.\n";
            }
            else {
                cout << "Failed to create prescription.\n";
            }
        }
        else if (c == 0) inDoc = false;
        else cout << "Invalid\n";
    }
    d->logout();
}

void HospitalSystem::patientPortal() {
    bool inPortal = true;
    while (inPortal) {
        cout << "\n--- Patient Portal ---\n1 Register\n2 Login\n0 Back\nChoose: ";
        int c; cin >> c; cin.ignore();
        if (c == 1) {
            int id; string name; cout << "ID: "; cin >> id; cin.ignore();
            if (idTaken(id)) { cout << "ID taken\n"; continue; }
            cout << "Name: "; getline(cin, name);
            patients.push_back(Patient(id, name));
            cout << "Registered\n";
        }
        else if (c == 2) {
            int id; cout << "Enter ID: "; cin >> id; cin.ignore();
            Patient* p = findPatient(id);
            if (!p) { cout << "Not found or disabled.\n"; continue; }
            p->login();
            patientMenu(p);
            p->logout();
        }
        else if (c == 0) inPortal = false;
        else cout << "Invalid\n";
    }
}

void HospitalSystem::patientMenu(Patient* p) {
    bool inP = true;
    while (inP) {
        cout << "\n--- Patient (" << p->name << ") ---\n1 Book Appointment\n2 View Appointment\n3 View Record\n0 Logout\nChoose: ";
        int c; cin >> c; cin.ignore();
        if (c == 1) bookAppointmentFor(p);
        else if (c == 2) { if (p->appointment) p->appointment->show(); else cout << "No appointment\n"; }
        else if (c == 3) p->viewRecord();
        else if (c == 0) inP = false;
        else cout << "Invalid\n";
    }
}

void HospitalSystem::bookAppointmentFor(Patient* p) {
    // collect specializations of active doctors
    vector<string> specs;
    for (auto& d : doctors) if (d.isActive) {
        bool found = false; for (auto& s : specs) if (s == d.specialization) { found = true; break; }
        if (!found) specs.push_back(d.specialization);
    }
    if (specs.empty()) { cout << "No doctors\n"; return; }
    cout << "\nSpecializations:\n";
    for (size_t i = 0; i < specs.size(); ++i) cout << i << ") " << specs[i] << "\n";
    int si; cout << "Choose: "; cin >> si; cin.ignore();
    if (si < 0 || si >= (int)specs.size()) { cout << "Invalid\n"; return; }
    string spec = specs[si];

    cout << "\nDoctors in " << spec << ":\n";
    for (auto& d : doctors) if (d.isActive && d.specialization == spec) cout << d.id << " - " << d.name << "\n";
    int did; cout << "Enter Doctor ID: "; cin >> did; cin.ignore();
    Doctor* d = findDoctor(did);
    if (!d || d->specialization != spec) { cout << "Doctor not available\n"; return; }

    // dates (7 days)
    const int DAYS = 7; vector<string> dates;
    for (int i = 0; i < DAYS; ++i) { string dt = getDate(i); dates.push_back(dt); cout << i << ") " << dt << "\n"; }
    int di; cout << "Choose date index: "; cin >> di; cin.ignore();
    if (di < 0 || di >= DAYS) { cout << "Invalid\n"; return; }
    string date = dates[di];

    // timeslots free -> now use TimeSlotProvider (OCP)
    vector<string> times = slotProvider->getSlots(); // <- OCP: HospitalSystem depends on abstraction
    vector<int> avail;
    cout << "\nSlots:\n";
    for (size_t i = 0; i < times.size(); ++i) {
        if (slotFree(d, date, times[i])) { cout << avail.size() << ") " << times[i] << "\n"; avail.push_back((int)i); }
    }
    if (avail.empty()) { cout << "No slots\n"; return; }
    int ti; cout << "Choose slot index: "; cin >> ti; cin.ignore();
    if (ti < 0 || ti >= (int)avail.size()) { cout << "Invalid\n"; return; }
    string time = times[avail[ti]];

    if (p->hasConflict(appointments, date, time)) { cout << "You already have appointment at that time\n"; return; }
    Appointment* a = p->makeAppointment(nextAppt++, date, time, d);
    if (!storeAppointment(a)) { delete a; cout << "Conflict\n"; return; }
    cout << "Booked:\n"; a->show();
}

string HospitalSystem::getDate(int offset) {
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now() + hours(24 * offset);
    time_t tt = system_clock::to_time_t(now);
    tm* t = localtime(&tt);
    char buf[11];
    sprintf(buf, "%04d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return string(buf);
}

vector<Patient*> HospitalSystem::allActivePatients() {
    vector<Patient*> out; for (auto& p : patients) if (p.isActive) out.push_back(&p); return out;
}
vector<Doctor*> HospitalSystem::allActiveDoctors() {
    vector<Doctor*> out; for (auto& d : doctors) if (d.isActive) out.push_back(&d); return out;
}

// ----------------- main -----------------
int main() {
    HospitalSystem sys;
    sys.run();
    return 0;
}
