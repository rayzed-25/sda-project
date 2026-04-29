#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <limits>
using namespace std;

struct Customer {
    int id;
    string name, cnic, phone, address, email, registrationDate;
};

struct Vehicle {
    int id, customerId, year;
    string make, model, registrationNumber, color;
};

struct InsurancePolicy {
    int id, vehicleId, customerId, salesmanId;
    string policyNumber, startDate, endDate, status;
    double premium, coverageAmount;
};

struct Claim {
    int id, policyId, customerId, surveyorId, workshopId;
    string claimDate, incidentDate, description, status;
    double estimatedLoss, approvedAmount;
};

struct InspectionReport {
    int id, claimId, surveyorId;
    string inspectionDate, findings, recommendation;
    double estimatedRepairCost;
};

struct Workshop {
    int id;
    string name, address, phone;
    bool isRegistered;
};

struct Staff {
    int id;
    string name, role, cnic, phone, email, username, passwordHash;
};

string dataDir() { return "data/"; }

string hashPassword(const string& plain) {
    unsigned long h = 5381;
    for (unsigned char c : plain) h = ((h << 5) + h) + c;
    ostringstream oss;
    oss << hex << h;
    return oss.str();
}

int nextId(const string& entity) {
    ifstream f(dataDir() + "ids.dat");
    string line;
    while (getline(f, line)) {
        istringstream ss(line);
        string key; int val;
        getline(ss, key, '=');
        ss >> val;
        if (key == entity) return val;
    }
    return 1;
}

void bumpId(const string& entity, int newVal) {
    ifstream f(dataDir() + "ids.dat");
    vector<pair<string, int>> entries;
    string line;
    bool found = false;
    while (getline(f, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string key; int val;
        getline(ss, key, '=');
        ss >> val;
        if (key == entity) { val = newVal; found = true; }
        entries.push_back({ key, val });
    }
    f.close();
    if (!found) entries.push_back({ entity, newVal });
    ofstream out(dataDir() + "ids.dat");
    for (auto& e : entries) out << e.first << "=" << e.second << "\n";
}

string esc(const string& s) {
    string r;
    for (char c : s) {
        if (c == '|') r += "\\p";
        else if (c == '\n') r += "\\n";
        else if (c == '\\') r += "\\\\";
        else r += c;
    }
    return r;
}

string unesc(const string& s) {
    string r;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char next = s[i + 1];
            if (next == 'p') { r += '|'; ++i; }
            else if (next == 'n') { r += '\n'; ++i; }
            else if (next == '\\') { r += '\\'; ++i; }
            else r += s[i];
        }
        else r += s[i];
    }
    return r;
}

vector<string> splitLine(const string& line) {
    vector<string> fields;
    istringstream ss(line);
    string tok;
    while (getline(ss, tok, '|')) fields.push_back(unesc(tok));
    return fields;
}

vector<Customer> loadCustomers() {
    vector<Customer> v;
    ifstream f(dataDir() + "customers.dat");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto flds = splitLine(line);
        if (flds.size() < 7) continue;
        Customer c;
        c.id = stoi(flds[0]); c.name = flds[1]; c.cnic = flds[2];
        c.phone = flds[3]; c.address = flds[4]; c.email = flds[5];
        c.registrationDate = flds[6];
        v.push_back(c);
    }
    return v;
}

void saveCustomers(const vector<Customer>& data) {
    ofstream f(dataDir() + "customers.dat");
    for (auto& c : data)
        f << c.id << "|" << esc(c.name) << "|" << esc(c.cnic) << "|"
        << esc(c.phone) << "|" << esc(c.address) << "|"
        << esc(c.email) << "|" << esc(c.registrationDate) << "\n";
}

vector<Vehicle> loadVehicles() {
    vector<Vehicle> v;
    ifstream f(dataDir() + "vehicles.dat");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto flds = splitLine(line);
        if (flds.size() < 7) continue;
        Vehicle vh;
        vh.id = stoi(flds[0]); vh.customerId = stoi(flds[1]);
        vh.make = flds[2]; vh.model = flds[3]; vh.year = stoi(flds[4]);
        vh.registrationNumber = flds[5]; vh.color = flds[6];
        v.push_back(vh);
    }
    return v;
}

void saveVehicles(const vector<Vehicle>& data) {
    ofstream f(dataDir() + "vehicles.dat");
    for (auto& vh : data)
        f << vh.id << "|" << vh.customerId << "|" << esc(vh.make) << "|"
        << esc(vh.model) << "|" << vh.year << "|"
        << esc(vh.registrationNumber) << "|" << esc(vh.color) << "\n";
}

vector<InsurancePolicy> loadPolicies() {
    vector<InsurancePolicy> v;
    ifstream f(dataDir() + "policies.dat");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto flds = splitLine(line);
        if (flds.size() < 10) continue;
        InsurancePolicy p;
        p.id = stoi(flds[0]); p.vehicleId = stoi(flds[1]);
        p.customerId = stoi(flds[2]); p.salesmanId = stoi(flds[3]);
        p.policyNumber = flds[4]; p.startDate = flds[5]; p.endDate = flds[6];
        p.premium = stod(flds[7]); p.coverageAmount = stod(flds[8]);
        p.status = flds[9];
        v.push_back(p);
    }
    return v;
}

void savePolicies(const vector<InsurancePolicy>& data) {
    ofstream f(dataDir() + "policies.dat");
    for (auto& p : data)
        f << p.id << "|" << p.vehicleId << "|" << p.customerId << "|"
        << p.salesmanId << "|" << esc(p.policyNumber) << "|"
        << esc(p.startDate) << "|" << esc(p.endDate) << "|"
        << p.premium << "|" << p.coverageAmount << "|" << esc(p.status) << "\n";
}

vector<Claim> loadClaims() {
    vector<Claim> v;
    ifstream f(dataDir() + "claims.dat");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto flds = splitLine(line);
        if (flds.size() < 11) continue;
        Claim c;
        c.id = stoi(flds[0]); c.policyId = stoi(flds[1]);
        c.customerId = stoi(flds[2]); c.claimDate = flds[3];
        c.incidentDate = flds[4]; c.description = flds[5];
        c.estimatedLoss = stod(flds[6]); c.approvedAmount = stod(flds[7]);
        c.status = flds[8]; c.surveyorId = stoi(flds[9]);
        c.workshopId = stoi(flds[10]);
        v.push_back(c);
    }
    return v;
}

void saveClaims(const vector<Claim>& data) {
    ofstream f(dataDir() + "claims.dat");
    for (auto& c : data)
        f << c.id << "|" << c.policyId << "|" << c.customerId << "|"
        << esc(c.claimDate) << "|" << esc(c.incidentDate) << "|"
        << esc(c.description) << "|" << c.estimatedLoss << "|"
        << c.approvedAmount << "|" << esc(c.status) << "|"
        << c.surveyorId << "|" << c.workshopId << "\n";
}

vector<InspectionReport> loadReports() {
    vector<InspectionReport> v;
    ifstream f(dataDir() + "reports.dat");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto flds = splitLine(line);
        if (flds.size() < 7) continue;
        InspectionReport r;
        r.id = stoi(flds[0]); r.claimId = stoi(flds[1]);
        r.surveyorId = stoi(flds[2]); r.inspectionDate = flds[3];
        r.findings = flds[4]; r.estimatedRepairCost = stod(flds[5]);
        r.recommendation = flds[6];
        v.push_back(r);
    }
    return v;
}

void saveReports(const vector<InspectionReport>& data) {
    ofstream f(dataDir() + "reports.dat");
    for (auto& r : data)
        f << r.id << "|" << r.claimId << "|" << r.surveyorId << "|"
        << esc(r.inspectionDate) << "|" << esc(r.findings) << "|"
        << r.estimatedRepairCost << "|" << esc(r.recommendation) << "\n";
}

vector<Workshop> loadWorkshops() {
    vector<Workshop> v;
    ifstream f(dataDir() + "workshops.dat");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto flds = splitLine(line);
        if (flds.size() < 5) continue;
        Workshop w;
        w.id = stoi(flds[0]); w.name = flds[1]; w.address = flds[2];
        w.phone = flds[3]; w.isRegistered = (flds[4] == "1");
        v.push_back(w);
    }
    return v;
}

void saveWorkshops(const vector<Workshop>& data) {
    ofstream f(dataDir() + "workshops.dat");
    for (auto& w : data)
        f << w.id << "|" << esc(w.name) << "|" << esc(w.address) << "|"
        << esc(w.phone) << "|" << (w.isRegistered ? "1" : "0") << "\n";
}

vector<Staff> loadStaff() {
    vector<Staff> v;
    ifstream f(dataDir() + "staff.dat");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto flds = splitLine(line);
        if (flds.size() < 8) continue;
        Staff s;
        s.id = stoi(flds[0]); s.name = flds[1]; s.role = flds[2];
        s.cnic = flds[3]; s.phone = flds[4]; s.email = flds[5];
        s.username = flds[6]; s.passwordHash = flds[7];
        v.push_back(s);
    }
    return v;
}

void saveStaff(const vector<Staff>& data) {
    ofstream f(dataDir() + "staff.dat");
    for (auto& s : data)
        f << s.id << "|" << esc(s.name) << "|" << esc(s.role) << "|"
        << esc(s.cnic) << "|" << esc(s.phone) << "|"
        << esc(s.email) << "|" << esc(s.username) << "|"
        << esc(s.passwordHash) << "\n";
}

class InsuranceSystem {
public:
    InsuranceSystem() : m_loggedIn(false) { m_currentUser = Staff{}; }

    bool login(const string& username, const string& password) {
        auto staff = loadStaff();
        string hashed = hashPassword(password);
        for (auto& s : staff) {
            if (s.username == username && s.passwordHash == hashed) {
                m_currentUser = s;
                m_loggedIn = true;
                return true;
            }
        }
        return false;
    }

    void logout() { m_loggedIn = false; m_currentUser = Staff{}; }
    Staff currentUser() const { return m_currentUser; }
    bool isLoggedIn() const { return m_loggedIn; }

    Customer registerCustomer(const string& name, const string& cnic,
        const string& phone, const string& address,
        const string& email) {
        auto customers = loadCustomers();
        int newId = nextId("customer");
        Customer c;
        c.id = newId; c.name = name; c.cnic = cnic;
        c.phone = phone; c.address = address; c.email = email;
        c.registrationDate = todayDate();
        customers.push_back(c);
        saveCustomers(customers);
        bumpId("customer", newId + 1);
        return c;
    }

    bool updateCustomer(int customerId, const string& phone,
        const string& address, const string& email) {
        auto customers = loadCustomers();
        for (auto& c : customers) {
            if (c.id == customerId) {
                c.phone = phone; c.address = address; c.email = email;
                saveCustomers(customers);
                return true;
            }
        }
        return false;
    }

    Customer getCustomer(int customerId) const {
        for (auto& c : loadCustomers())
            if (c.id == customerId) return c;
        throw runtime_error("Customer not found: " + to_string(customerId));
    }

    vector<Customer> searchCustomers(const string& kw) const {
        vector<Customer> result;
        for (auto& c : loadCustomers())
            if (c.name.find(kw) != string::npos ||
                c.cnic.find(kw) != string::npos ||
                c.phone.find(kw) != string::npos)
                result.push_back(c);
        return result;
    }

    vector<Customer> getAllCustomers() const { return loadCustomers(); }

    vector<Customer> getNewCustomers(const string& month) const {
        vector<Customer> result;
        for (auto& c : loadCustomers())
            if (c.registrationDate.substr(0, 7) == month)
                result.push_back(c);
        return result;
    }

    Vehicle addVehicle(int customerId, const string& make, const string& model,
        int year, const string& regNumber, const string& color) {
        auto vehicles = loadVehicles();
        int newId = nextId("vehicle");
        Vehicle v;
        v.id = newId; v.customerId = customerId; v.make = make;
        v.model = model; v.year = year; v.registrationNumber = regNumber;
        v.color = color;
        vehicles.push_back(v);
        saveVehicles(vehicles);
        bumpId("vehicle", newId + 1);
        return v;
    }

    vector<Vehicle> getVehiclesByCustomer(int customerId) const {
        vector<Vehicle> result;
        for (auto& v : loadVehicles())
            if (v.customerId == customerId) result.push_back(v);
        return result;
    }

    Vehicle getVehicle(int vehicleId) const {
        for (auto& v : loadVehicles())
            if (v.id == vehicleId) return v;
        throw runtime_error("Vehicle not found: " + to_string(vehicleId));
    }

    InsurancePolicy issuePolicy(int vehicleId, int customerId, int salesmanId,
        const string& startDate, const string& endDate,
        double premium, double coverageAmount) {
        auto policies = loadPolicies();
        int newId = nextId("policy");
        InsurancePolicy p;
        p.id = newId; p.vehicleId = vehicleId; p.customerId = customerId;
        p.salesmanId = salesmanId; p.policyNumber = makePolicyNumber();
        p.startDate = startDate; p.endDate = endDate;
        p.premium = premium; p.coverageAmount = coverageAmount;
        p.status = "Active";
        policies.push_back(p);
        savePolicies(policies);
        bumpId("policy", newId + 1);
        return p;
    }

    bool cancelPolicy(int policyId) {
        auto policies = loadPolicies();
        for (auto& p : policies) {
            if (p.id == policyId) {
                p.status = "Cancelled";
                savePolicies(policies);
                return true;
            }
        }
        return false;
    }

    InsurancePolicy getPolicy(int policyId) const {
        for (auto& p : loadPolicies())
            if (p.id == policyId) return p;
        throw runtime_error("Policy not found: " + to_string(policyId));
    }

    vector<InsurancePolicy> getPoliciesByCustomer(int customerId) const {
        vector<InsurancePolicy> result;
        for (auto& p : loadPolicies())
            if (p.customerId == customerId) result.push_back(p);
        return result;
    }

    vector<InsurancePolicy> getActivePolicies() const {
        vector<InsurancePolicy> result;
        for (auto& p : loadPolicies())
            if (p.status == "Active") result.push_back(p);
        return result;
    }

    Claim fileClaim(int policyId, int customerId, const string& incidentDate,
        const string& description, double estimatedLoss) {
        auto claims = loadClaims();
        int newId = nextId("claim");
        Claim c;
        c.id = newId; c.policyId = policyId; c.customerId = customerId;
        c.claimDate = todayDate(); c.incidentDate = incidentDate;
        c.description = description; c.estimatedLoss = estimatedLoss;
        c.approvedAmount = 0.0; c.status = "Pending";
        c.surveyorId = 0; c.workshopId = 0;
        claims.push_back(c);
        saveClaims(claims);
        bumpId("claim", newId + 1);
        return c;
    }

    bool assignSurveyor(int claimId, int surveyorId) {
        auto claims = loadClaims();
        for (auto& c : claims) {
            if (c.id == claimId) {
                c.surveyorId = surveyorId;
                c.status = "UnderReview";
                saveClaims(claims);
                return true;
            }
        }
        return false;
    }

    bool assignWorkshop(int claimId, int workshopId) {
        auto claims = loadClaims();
        for (auto& c : claims) {
            if (c.id == claimId) {
                c.workshopId = workshopId;
                saveClaims(claims);
                return true;
            }
        }
        return false;
    }

    bool approveClaim(int claimId, double approvedAmount) {
        auto claims = loadClaims();
        for (auto& c : claims) {
            if (c.id == claimId) {
                c.approvedAmount = approvedAmount;
                c.status = "Approved";
                saveClaims(claims);
                return true;
            }
        }
        return false;
    }

    bool rejectClaim(int claimId) {
        auto claims = loadClaims();
        for (auto& c : claims) {
            if (c.id == claimId) {
                c.status = "Rejected";
                saveClaims(claims);
                return true;
            }
        }
        return false;
    }

    Claim getClaim(int claimId) const {
        for (auto& c : loadClaims())
            if (c.id == claimId) return c;
        throw runtime_error("Claim not found: " + to_string(claimId));
    }

    vector<Claim> getPendingClaims() const {
        vector<Claim> result;
        for (auto& c : loadClaims())
            if (c.status == "Pending" || c.status == "UnderReview")
                result.push_back(c);
        return result;
    }

    vector<Claim> getClaimsByCustomer(int customerId) const {
        vector<Claim> result;
        for (auto& c : loadClaims())
            if (c.customerId == customerId) result.push_back(c);
        return result;
    }

    vector<Claim> getClaimsBySurveyor(int surveyorId) const {
        vector<Claim> result;
        for (auto& c : loadClaims())
            if (c.surveyorId == surveyorId) result.push_back(c);
        return result;
    }

    vector<Claim> getAllClaims() const { return loadClaims(); }

    InspectionReport submitInspectionReport(int claimId, int surveyorId,
        const string& inspectionDate,
        const string& findings,
        double estimatedRepairCost,
        const string& recommendation) {
        auto reports = loadReports();
        int newId = nextId("report");
        InspectionReport r;
        r.id = newId; r.claimId = claimId; r.surveyorId = surveyorId;
        r.inspectionDate = inspectionDate; r.findings = findings;
        r.estimatedRepairCost = estimatedRepairCost;
        r.recommendation = recommendation;
        reports.push_back(r);
        saveReports(reports);
        bumpId("report", newId + 1);
        auto claims = loadClaims();
        for (auto& c : claims)
            if (c.id == claimId) { c.status = "UnderReview"; break; }
        saveClaims(claims);
        return r;
    }

    InspectionReport getReportByClaim(int claimId) const {
        for (auto& r : loadReports())
            if (r.claimId == claimId) return r;
        throw runtime_error("No report for claim: " + to_string(claimId));
    }

    vector<InspectionReport> getAllReports() const { return loadReports(); }

    Workshop registerWorkshop(const string& name, const string& address,
        const string& phone) {
        auto workshops = loadWorkshops();
        int newId = nextId("workshop");
        Workshop w;
        w.id = newId; w.name = name; w.address = address;
        w.phone = phone; w.isRegistered = true;
        workshops.push_back(w);
        saveWorkshops(workshops);
        bumpId("workshop", newId + 1);
        return w;
    }

    vector<Workshop> getRegisteredWorkshops() const {
        vector<Workshop> result;
        for (auto& w : loadWorkshops())
            if (w.isRegistered) result.push_back(w);
        return result;
    }

    Workshop getWorkshop(int workshopId) const {
        for (auto& w : loadWorkshops())
            if (w.id == workshopId) return w;
        throw runtime_error("Workshop not found: " + to_string(workshopId));
    }

    Staff addStaff(const string& name, const string& role, const string& cnic,
        const string& phone, const string& email,
        const string& username, const string& password) {
        auto staff = loadStaff();
        int newId = nextId("staff");
        Staff s;
        s.id = newId; s.name = name; s.role = role; s.cnic = cnic;
        s.phone = phone; s.email = email; s.username = username;
        s.passwordHash = hashPassword(password);
        staff.push_back(s);
        saveStaff(staff);
        bumpId("staff", newId + 1);
        return s;
    }

    vector<Staff> getAllStaff() const { return loadStaff(); }

    vector<Staff> getStaffByRole(const string& role) const {
        vector<Staff> result;
        for (auto& s : loadStaff())
            if (s.role == role) result.push_back(s);
        return result;
    }

    string reportNewCustomers(const string& month) const {
        ostringstream oss;
        oss << "NEW CUSTOMERS - " << month << "\n" << string(60, '=') << "\n";
        auto customers = getNewCustomers(month);
        if (customers.empty()) { oss << "None found.\n"; return oss.str(); }
        oss << left << setw(5) << "ID" << setw(22) << "Name"
            << setw(16) << "CNIC" << setw(14) << "Phone" << "Date\n"
            << string(60, '-') << "\n";
        for (auto& c : customers)
            oss << setw(5) << c.id << setw(22) << c.name
            << setw(16) << c.cnic << setw(14) << c.phone
            << c.registrationDate << "\n";
        oss << "\nTotal: " << customers.size() << "\n";
        return oss.str();
    }

    string reportPendingClaims() const {
        ostringstream oss;
        oss << "PENDING CLAIMS\n" << string(60, '=') << "\n";
        auto claims = getPendingClaims();
        if (claims.empty()) { oss << "No pending claims.\n"; return oss.str(); }
        oss << left << setw(5) << "ID" << setw(13) << "Filed"
            << setw(13) << "Status" << setw(12) << "Est.Loss" << "Description\n"
            << string(60, '-') << "\n";
        for (auto& c : claims)
            oss << setw(5) << c.id << setw(13) << c.claimDate
            << setw(13) << c.status << setw(12) << fixed << setprecision(2)
            << c.estimatedLoss << c.description.substr(0, 30) << "\n";
        oss << "\nTotal: " << claims.size() << "\n";
        return oss.str();
    }

    string reportInspections() const {
        ostringstream oss;
        oss << "INSPECTION REPORTS\n" << string(60, '=') << "\n";
        auto reports = getAllReports();
        if (reports.empty()) { oss << "No reports.\n"; return oss.str(); }
        for (auto& r : reports) {
            oss << "Report #" << r.id << "  Claim #" << r.claimId
                << "  Surveyor #" << r.surveyorId << "\n"
                << "  Date       : " << r.inspectionDate << "\n"
                << "  Findings   : " << r.findings << "\n"
                << "  Cost       : Rs." << fixed << setprecision(2)
                << r.estimatedRepairCost << "\n"
                << "  Recommend  : " << r.recommendation << "\n"
                << string(40, '-') << "\n";
        }
        oss << "Total: " << reports.size() << "\n";
        return oss.str();
    }

    string reportCustomerClaims(int customerId) const {
        ostringstream oss;
        try {
            Customer cust = getCustomer(customerId);
            oss << "CLAIMS - " << cust.name << " (ID:" << cust.id << ")\n"
                << string(60, '=') << "\n";
            auto claims = getClaimsByCustomer(customerId);
            if (claims.empty()) { oss << "No claims.\n"; return oss.str(); }
            for (auto& c : claims)
                oss << "Claim #" << c.id << "\n"
                << "  Policy    : " << c.policyId << "\n"
                << "  Incident  : " << c.incidentDate << "\n"
                << "  Filed     : " << c.claimDate << "\n"
                << "  Status    : " << c.status << "\n"
                << "  Est. Loss : Rs." << fixed << setprecision(2)
                << c.estimatedLoss << "\n"
                << "  Approved  : Rs." << c.approvedAmount << "\n"
                << "  Desc      : " << c.description << "\n"
                << string(40, '-') << "\n";
            oss << "Total: " << claims.size() << "\n";
        }
        catch (exception& e) { oss << "Error: " << e.what() << "\n"; }
        return oss.str();
    }

    string reportPolicySummary() const {
        ostringstream oss;
        auto policies = loadPolicies();
        int active = 0, expired = 0, cancelled = 0;
        double totalPremium = 0, totalCoverage = 0;
        for (auto& p : policies) {
            if (p.status == "Active") ++active;
            if (p.status == "Expired") ++expired;
            if (p.status == "Cancelled") ++cancelled;
            totalPremium += p.premium;
            totalCoverage += p.coverageAmount;
        }
        oss << "POLICY SUMMARY\n" << string(60, '=') << "\n"
            << "Total Policies : " << policies.size() << "\n"
            << "Active         : " << active << "\n"
            << "Expired        : " << expired << "\n"
            << "Cancelled      : " << cancelled << "\n"
            << "Total Premium  : Rs." << fixed << setprecision(2) << totalPremium << "\n"
            << "Total Coverage : Rs." << totalCoverage << "\n";
        return oss.str();
    }

private:
    Staff m_currentUser;
    bool m_loggedIn;

    string todayDate() const {
        time_t t = time(nullptr);
        tm* tm = localtime(&t);
        ostringstream oss;
        oss << (tm->tm_year + 1900) << "-"
            << setw(2) << setfill('0') << (tm->tm_mon + 1) << "-"
            << setw(2) << setfill('0') << tm->tm_mday;
        return oss.str();
    }

    string makePolicyNumber() const {
        ostringstream oss;
        oss << "POL-" << setw(6) << setfill('0') << nextId("policy");
        return oss.str();
    }
};

class UI {
public:
    UI(InsuranceSystem& sys) : m_sys(sys) {}

    void run() {
        setupDefaultData();
        while (true) {
            if (!m_sys.isLoggedIn()) {
                showLoginScreen();
                if (!m_sys.isLoggedIn()) break;
            }
            mainMenu();
        }
        cout << "\nGoodbye.\n";
    }

private:
    InsuranceSystem& m_sys;

    void setupDefaultData() {
        if (loadStaff().empty()) {
            cout << "First run: creating default admin account (admin/admin123)\n";
            m_sys.addStaff("Admin Manager", "Manager", "00000-0000000-0",
                "000-0000000", "admin@insurance.com", "admin", "admin123");
        }
    }

    void showLoginScreen() {
        printHeader("AUTO INSURANCE MANAGEMENT SYSTEM");
        string user = readLine("Username: ");
        string pass = readLine("Password: ");
        if (!m_sys.login(user, pass)) {
            cout << "Invalid credentials.\n";
            pause();
        }
        else {
            cout << "Welcome, " << m_sys.currentUser().name
                << " [" << m_sys.currentUser().role << "]\n";
            pause();
        }
    }

    void mainMenu() {
        string role = m_sys.currentUser().role;
        if (role == "Manager") managerMenu();
        else if (role == "Salesman") salesmanMenu();
        else if (role == "Surveyor") surveyorMenu();
        else { cout << "Unknown role.\n"; m_sys.logout(); }
    }

    void managerMenu() {
        while (true) {
            printHeader("MANAGER MENU");
            cout << "1. Customer Management\n"
                << "2. Policy Management\n"
                << "3. Claim Management\n"
                << "4. Workshop Management\n"
                << "5. Staff Management\n"
                << "6. Reports\n"
                << "0. Logout\n";
            int ch = readInt("Choice: ");
            if (ch == 1) managerCustomerMenu();
            else if (ch == 2) managerPolicyMenu();
            else if (ch == 3) managerClaimMenu();
            else if (ch == 4) managerWorkshopMenu();
            else if (ch == 5) managerStaffMenu();
            else if (ch == 6) managerReportsMenu();
            else if (ch == 0) { m_sys.logout(); return; }
            else cout << "Invalid choice.\n";
        }
    }

    void salesmanMenu() {
        while (true) {
            printHeader("SALESMAN MENU");
            cout << "1. Customer Management\n"
                << "2. Vehicle Management\n"
                << "3. Policy Management\n"
                << "0. Logout\n";
            int ch = readInt("Choice: ");
            if (ch == 1) salesmanCustomerMenu();
            else if (ch == 2) salesmanVehicleMenu();
            else if (ch == 3) salesmanPolicyMenu();
            else if (ch == 0) { m_sys.logout(); return; }
            else cout << "Invalid choice.\n";
        }
    }

    void surveyorMenu() {
        while (true) {
            printHeader("SURVEYOR MENU");
            cout << "1. My Assigned Claims\n"
                << "2. Submit Inspection Report\n"
                << "0. Logout\n";
            int ch = readInt("Choice: ");
            if (ch == 1) surveyorClaimMenu();
            else if (ch == 2) surveyorInspectionMenu();
            else if (ch == 0) { m_sys.logout(); return; }
            else cout << "Invalid choice.\n";
        }
    }

    void managerCustomerMenu() {
        while (true) {
            printHeader("CUSTOMER MANAGEMENT");
            cout << "1. View All Customers\n"
                << "2. Search Customer\n"
                << "3. View Customer Claims History\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                for (auto& c : m_sys.getAllCustomers()) printCustomer(c);
                pause();
            }
            else if (ch == 2) {
                string kw = readLine("Search (name/cnic/phone): ");
                auto res = m_sys.searchCustomers(kw);
                if (res.empty()) cout << "No results.\n";
                else for (auto& c : res) printCustomer(c);
                pause();
            }
            else if (ch == 3) {
                int id = readInt("Customer ID: ");
                cout << m_sys.reportCustomerClaims(id);
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void managerPolicyMenu() {
        while (true) {
            printHeader("POLICY MANAGEMENT");
            cout << "1. View Active Policies\n"
                << "2. Cancel Policy\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                for (auto& p : m_sys.getActivePolicies()) printPolicy(p);
                pause();
            }
            else if (ch == 2) {
                int id = readInt("Policy ID: ");
                cout << (m_sys.cancelPolicy(id) ? "Cancelled.\n" : "Not found.\n");
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void managerClaimMenu() {
        while (true) {
            printHeader("CLAIM MANAGEMENT");
            cout << "1. View All Claims\n"
                << "2. View Pending Claims\n"
                << "3. Assign Surveyor\n"
                << "4. Assign Workshop\n"
                << "5. View Inspection Report\n"
                << "6. Approve Claim\n"
                << "7. Reject Claim\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                for (auto& c : m_sys.getAllClaims()) printClaim(c);
                pause();
            }
            else if (ch == 2) {
                for (auto& c : m_sys.getPendingClaims()) printClaim(c);
                pause();
            }
            else if (ch == 3) {
                int cid = readInt("Claim ID: ");
                cout << "Available Surveyors:\n";
                for (auto& s : m_sys.getStaffByRole("Surveyor"))
                    cout << "  #" << s.id << " " << s.name << "\n";
                int sid = readInt("Surveyor ID: ");
                cout << (m_sys.assignSurveyor(cid, sid) ? "Assigned.\n" : "Failed.\n");
                pause();
            }
            else if (ch == 4) {
                int cid = readInt("Claim ID: ");
                cout << "Registered Workshops:\n";
                for (auto& w : m_sys.getRegisteredWorkshops())
                    cout << "  #" << w.id << " " << w.name << "\n";
                int wid = readInt("Workshop ID: ");
                cout << (m_sys.assignWorkshop(cid, wid) ? "Assigned.\n" : "Failed.\n");
                pause();
            }
            else if (ch == 5) {
                int cid = readInt("Claim ID: ");
                try { printReport(m_sys.getReportByClaim(cid)); }
                catch (exception& e) { cout << e.what() << "\n"; }
                pause();
            }
            else if (ch == 6) {
                int cid = readInt("Claim ID: ");
                try {
                    printReport(m_sys.getReportByClaim(cid));
                    double amt = readDouble("Approved Amount: Rs.");
                    cout << (m_sys.approveClaim(cid, amt) ? "Approved.\n" : "Failed.\n");
                }
                catch (exception& e) { cout << e.what() << "\n"; }
                pause();
            }
            else if (ch == 7) {
                int cid = readInt("Claim ID: ");
                cout << (m_sys.rejectClaim(cid) ? "Rejected.\n" : "Failed.\n");
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void managerWorkshopMenu() {
        while (true) {
            printHeader("WORKSHOP MANAGEMENT");
            cout << "1. View Registered Workshops\n"
                << "2. Register New Workshop\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                for (auto& w : m_sys.getRegisteredWorkshops()) printWorkshop(w);
                pause();
            }
            else if (ch == 2) {
                string name = readLine("Workshop Name: ");
                string addr = readLine("Address: ");
                string phone = readLine("Phone: ");
                auto w = m_sys.registerWorkshop(name, addr, phone);
                cout << "Registered: #" << w.id << " " << w.name << "\n";
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void managerStaffMenu() {
        while (true) {
            printHeader("STAFF MANAGEMENT");
            cout << "1. View All Staff\n"
                << "2. Add Staff Member\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                for (auto& s : m_sys.getAllStaff()) printStaff(s);
                pause();
            }
            else if (ch == 2) {
                string name = readLine("Name: ");
                cout << "Role (Manager/Salesman/Surveyor): ";
                string role = readLine("");
                string cnic = readLine("CNIC: ");
                string phone = readLine("Phone: ");
                string email = readLine("Email: ");
                string user = readLine("Username: ");
                string pass = readLine("Password: ");
                auto s = m_sys.addStaff(name, role, cnic, phone, email, user, pass);
                cout << "Added: #" << s.id << " " << s.name << "\n";
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void managerReportsMenu() {
        while (true) {
            printHeader("REPORTS");
            cout << "1. New Customers (by month)\n"
                << "2. Pending Claims\n"
                << "3. Inspection Reports\n"
                << "4. Customer Claims History\n"
                << "5. Policy Summary\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                string month = readLine("Month (YYYY-MM): ");
                cout << m_sys.reportNewCustomers(month);
                pause();
            }
            else if (ch == 2) {
                cout << m_sys.reportPendingClaims();
                pause();
            }
            else if (ch == 3) {
                cout << m_sys.reportInspections();
                pause();
            }
            else if (ch == 4) {
                int id = readInt("Customer ID: ");
                cout << m_sys.reportCustomerClaims(id);
                pause();
            }
            else if (ch == 5) {
                cout << m_sys.reportPolicySummary();
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void salesmanCustomerMenu() {
        while (true) {
            printHeader("CUSTOMER MANAGEMENT");
            cout << "1. Register New Customer\n"
                << "2. Search Customer\n"
                << "3. Update Customer Info\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                string name = readLine("Name: ");
                string cnic = readLine("CNIC: ");
                string phone = readLine("Phone: ");
                string addr = readLine("Address: ");
                string email = readLine("Email: ");
                auto c = m_sys.registerCustomer(name, cnic, phone, addr, email);
                cout << "Registered: #" << c.id << " " << c.name << "\n";
                pause();
            }
            else if (ch == 2) {
                string kw = readLine("Search: ");
                auto res = m_sys.searchCustomers(kw);
                if (res.empty()) cout << "No results.\n";
                else for (auto& c : res) printCustomer(c);
                pause();
            }
            else if (ch == 3) {
                int id = readInt("Customer ID: ");
                string phone = readLine("New Phone: ");
                string addr = readLine("New Address: ");
                string email = readLine("New Email: ");
                cout << (m_sys.updateCustomer(id, phone, addr, email) ? "Updated.\n" : "Not found.\n");
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void salesmanVehicleMenu() {
        while (true) {
            printHeader("VEHICLE MANAGEMENT");
            cout << "1. Add Vehicle\n"
                << "2. View Customer Vehicles\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                int custId = readInt("Customer ID: ");
                string make = readLine("Make: ");
                string model = readLine("Model: ");
                int year = readInt("Year: ");
                string reg = readLine("Registration No: ");
                string color = readLine("Color: ");
                auto v = m_sys.addVehicle(custId, make, model, year, reg, color);
                cout << "Added: #" << v.id << " " << v.make << " " << v.model << "\n";
                pause();
            }
            else if (ch == 2) {
                int custId = readInt("Customer ID: ");
                auto vehicles = m_sys.getVehiclesByCustomer(custId);
                if (vehicles.empty()) cout << "No vehicles.\n";
                else for (auto& v : vehicles) printVehicle(v);
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void salesmanPolicyMenu() {
        while (true) {
            printHeader("POLICY MANAGEMENT");
            cout << "1. Issue Policy\n"
                << "2. View Customer Policies\n"
                << "3. File Claim\n"
                << "0. Back\n";
            int ch = readInt("Choice: ");
            if (ch == 1) {
                int custId = readInt("Customer ID: ");
                auto vehicles = m_sys.getVehiclesByCustomer(custId);
                if (vehicles.empty()) { cout << "No vehicles for this customer.\n"; pause(); continue; }
                cout << "Vehicles:\n";
                for (auto& v : vehicles) printVehicle(v);
                int vehId = readInt("Vehicle ID: ");
                string start = readLine("Start Date (YYYY-MM-DD): ");
                string end = readLine("End Date (YYYY-MM-DD): ");
                double premium = readDouble("Premium (Rs.): ");
                double coverage = readDouble("Coverage Amount (Rs.): ");
                auto p = m_sys.issuePolicy(vehId, custId, m_sys.currentUser().id,
                    start, end, premium, coverage);
                cout << "Policy issued: " << p.policyNumber << "\n";
                pause();
            }
            else if (ch == 2) {
                int custId = readInt("Customer ID: ");
                auto policies = m_sys.getPoliciesByCustomer(custId);
                if (policies.empty()) cout << "No policies.\n";
                else for (auto& p : policies) printPolicy(p);
                pause();
            }
            else if (ch == 3) {
                int policyId = readInt("Policy ID: ");
                int custId = readInt("Customer ID: ");
                string incDate = readLine("Incident Date (YYYY-MM-DD): ");
                string desc = readLine("Description: ");
                double loss = readDouble("Estimated Loss (Rs.): ");
                auto c = m_sys.fileClaim(policyId, custId, incDate, desc, loss);
                cout << "Claim #" << c.id << " filed successfully.\n";
                pause();
            }
            else if (ch == 0) return;
        }
    }

    void surveyorClaimMenu() {
        printHeader("MY ASSIGNED CLAIMS");
        auto claims = m_sys.getClaimsBySurveyor(m_sys.currentUser().id);
        if (claims.empty()) cout << "No claims assigned.\n";
        else for (auto& c : claims) printClaim(c);
        pause();
    }

    void surveyorInspectionMenu() {
        printHeader("SUBMIT INSPECTION REPORT");
        int claimId = readInt("Claim ID: ");
        string date = readLine("Inspection Date (YYYY-MM-DD): ");
        string findings = readLine("Findings: ");
        double cost = readDouble("Estimated Repair Cost (Rs.): ");
        cout << "Recommendation (Approve/Reject/PartialApprove): ";
        string rec = readLine("");
        auto r = m_sys.submitInspectionReport(claimId, m_sys.currentUser().id,
            date, findings, cost, rec);
        cout << "Report #" << r.id << " submitted.\n";
        pause();
    }

    int readInt(const string& prompt) {
        while (true) {
            cout << prompt;
            int v;
            if (cin >> v) { cin.ignore(numeric_limits<streamsize>::max(), '\n'); return v; }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number.\n";
        }
    }

    double readDouble(const string& prompt) {
        while (true) {
            cout << prompt;
            double v;
            if (cin >> v) { cin.ignore(numeric_limits<streamsize>::max(), '\n'); return v; }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number.\n";
        }
    }

    string readLine(const string& prompt) {
        if (!prompt.empty()) cout << prompt;
        string line;
        getline(cin, line);
        return line;
    }

    void pause() { readLine("Press Enter to continue..."); }

    void printLine(char c = '-', int w = 60) { cout << string(w, c) << "\n"; }

    void printHeader(const string& title) {
        cout << "\n"; printLine('=');
        cout << "  " << title << "\n"; printLine('=');
    }

    void printCustomer(const Customer& c) {
        cout << "[#" << c.id << "] " << c.name << " | CNIC: " << c.cnic
            << " | Phone: " << c.phone << " | Reg: " << c.registrationDate << "\n";
    }

    void printVehicle(const Vehicle& v) {
        cout << "[#" << v.id << "] " << v.year << " " << v.make << " " << v.model
            << " | Reg: " << v.registrationNumber << " | Color: " << v.color << "\n";
    }

    void printPolicy(const InsurancePolicy& p) {
        cout << "[#" << p.id << "] " << p.policyNumber << " | Status: " << p.status
            << " | " << p.startDate << " to " << p.endDate
            << " | Premium: Rs." << fixed << setprecision(2) << p.premium << "\n";
    }

    void printClaim(const Claim& c) {
        cout << "[#" << c.id << "] Policy#" << c.policyId
            << " | Status: " << c.status
            << " | Filed: " << c.claimDate
            << " | Loss: Rs." << fixed << setprecision(2) << c.estimatedLoss << "\n"
            << "  Desc: " << c.description << "\n";
    }

    void printReport(const InspectionReport& r) {
        cout << "Report #" << r.id << " | Claim #" << r.claimId
            << " | Surveyor #" << r.surveyorId << "\n"
            << "  Date      : " << r.inspectionDate << "\n"
            << "  Findings  : " << r.findings << "\n"
            << "  Cost      : Rs." << fixed << setprecision(2) << r.estimatedRepairCost << "\n"
            << "  Recommend : " << r.recommendation << "\n";
    }

    void printWorkshop(const Workshop& w) {
        cout << "[#" << w.id << "] " << w.name << " | " << w.address
            << " | Phone: " << w.phone << "\n";
    }

    void printStaff(const Staff& s) {
        cout << "[#" << s.id << "] " << s.name << " | Role: " << s.role
            << " | User: " << s.username << " | Phone: " << s.phone << "\n";
    }
};

int main() {
    system("mkdir -p data");
    InsuranceSystem sys;
    UI ui(sys);
    ui.run();
    return 0;
}