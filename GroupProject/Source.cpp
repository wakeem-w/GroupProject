#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class User {
protected:
    int login_id;
    std::string password;
    std::string first_name;
    std::string last_name;

public:
    virtual bool authenticate(int id, const std::string& pass) = 0;
    virtual std::string getRole() const = 0;
    virtual ~User() = default;
    int getLoginId() const { return login_id; }
    std::string getFullName() const { return first_name + " " + last_name; }
};

class Physician : public User {
    std::string title;
    std::string expertise;
public:
    Physician(int id, const std::string& pw, const std::string& fn, const std::string& ln,
        const std::string& t, const std::string& exp) {
        login_id = id;
        password = pw;
        first_name = fn;
        last_name = ln;
        title = t;
        expertise = exp;
    }

    bool authenticate(int id, const std::string& pass) override {
        return id == login_id && password == pass;
    }

    std::string getRole() const override {
        return "Physician";
    }

    void display() const {
        std::cout << login_id << ": Dr. " << first_name << " " << last_name << " (" << expertise << ")\n";
    }
};

class Patient : public User {
    double weight;
    double height;
    std::string blood_pressure;
    int physician_id;
public:
    Patient(int id, const std::string& pw, const std::string& fn, const std::string& ln,
        double w, double h, const std::string& bp, int pid) {
        login_id = id;
        password = pw;
        first_name = fn;
        last_name = ln;
        weight = w;
        height = h;
        blood_pressure = bp;
        physician_id = pid;
    }

    bool authenticate(int id, const std::string& pass) override {
        return id == login_id && password == pass;
    }

    std::string getRole() const override {
        return "Patient";
    }

    void display() const {
        std::cout << login_id << ": " << first_name << " " << last_name
            << " | Weight: " << weight << ", Height: " << height
            << ", BP: " << blood_pressure << ", Physician ID: " << physician_id << "\n";
    }

    void assignPhysician(int new_id) {
        physician_id = new_id;
    }

    // Accessors for saving to file
    std::string getPassword() const { return password; }
    std::string getFirstName() const { return first_name; }
    std::string getLastName() const { return last_name; }
    double getWeight() const { return weight; }
    double getHeight() const { return height; }
    std::string getBloodPressure() const { return blood_pressure; }
    int getPhysicianId() const { return physician_id; }
};



std::vector<Physician> loadPhysicians(const std::string& filename) {
    std::vector<Physician> list;
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string id_str, pw, fn, ln, title, exp;
        getline(ss, id_str, ',');
        getline(ss, pw, ',');
        getline(ss, fn, ',');
        getline(ss, ln, ',');
        getline(ss, title, ',');
        getline(ss, exp, ',');

        int id = stoi(id_str);
        list.emplace_back(id, pw, fn, ln, title, exp);
    }
    return list;
}

std::vector<Patient> loadPatients(const std::string& filename) {
    std::vector<Patient> list;
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string id_str, pw, fn, ln, w_str, h_str, bp, pid_str;
        getline(ss, id_str, ',');
        getline(ss, pw, ',');
        getline(ss, fn, ',');
        getline(ss, ln, ',');
        getline(ss, w_str, ',');
        getline(ss, h_str, ',');
        getline(ss, bp, ',');
        getline(ss, pid_str, ',');

        int id = stoi(id_str);
        double weight = stod(w_str);
        double height = stod(h_str);
        int pid = stoi(pid_str);
        list.emplace_back(id, pw, fn, ln, weight, height, bp, pid);
    }
    return list;
}

void savePatientsToFile(const std::string& filename, const std::vector<Patient>& patients) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error saving to file: " << filename << "\n";
        return;
    }

    file << "login_id,password,first_name,last_name,weight,height,blood_pressure,physician_id\n";
    for (const auto& p : patients) {
        file << p.getLoginId() << ","
            << p.getPassword() << ","
            << p.getFirstName() << ","
            << p.getLastName() << ","
            << p.getWeight() << ","
            << p.getHeight() << ","
            << p.getBloodPressure() << ","
            << p.getPhysicianId() << "\n";
    }

    file.close();
    std::cout << "Patient data saved to file successfully.\n";
}

int main() {
    std::vector<Physician> physicians = loadPhysicians("Physician.csv");
    std::vector<Patient> patients = loadPatients("Patient.csv");

    int input_id;
    std::string input_pw;
    std::cout << "Login ID: ";
    std::cin >> input_id;
    std::cout << "Password: ";
    std::cin >> input_pw;

    for (auto& p : physicians) {
        if (p.authenticate(input_id, input_pw)) {
            std::cout << "Welcome Dr. " << p.getFullName() << "!\n";
            return 0;
        }
    }

    for (auto& p : patients) {
        if (p.authenticate(input_id, input_pw)) {
            std::cout << "Welcome " << p.getFullName() << "!\n";

            std::cout << "\nAvailable Physicians:\n";
            for (const auto& doc : physicians) {
                std::cout << doc.getLoginId() << ": Dr. " << doc.getFullName() << "\n";
            }

            std::cout << "Enter new physician ID to assign: ";
            int new_phys_id;
            std::cin >> new_phys_id;

            p.assignPhysician(new_phys_id);
            std::cout << "Physician updated successfully.\n";

            savePatientsToFile("Patient.csv", patients); // Save to file after update
            return 0;
        }
    }

    std::cout << "Login failed.\n";
    return 0;
}


