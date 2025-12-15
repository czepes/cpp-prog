#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

class Student {
private:
  string firstname;
  string lastname;
  uint year;
  double avg_grade;

public:
  Student() : firstname(""), lastname(""), year(0), avg_grade(0.) {};
  Student(const string &firstname, const string &lastname, int year,
          double avg_grade)
      : firstname(firstname), lastname(lastname), year(clamp(year, 0, 4)),
        avg_grade(clamp(avg_grade, 0., 5.)) {};

  string get_firstname() const { return firstname; }
  string get_lastname() const { return lastname; }
  uint get_year() const { return year; }
  double get_grade() const { return avg_grade; }

  void set_firstname(const string &firstname) { this->firstname = firstname; }
  void set_lastname(const string &lastname) { this->lastname = lastname; }
  void set_year(int year) { this->year = clamp(year, 0, 4); }
  void set_grade(double avg_grade) {
    this->avg_grade = clamp(avg_grade, 0., 5.);
  }

  string to_string() {
    return firstname + " " + lastname + " " + std::to_string(year) + " " +
           std::to_string(avg_grade);
  }

  friend ostream &operator<<(ostream &os, const Student &student) {
    os << student.firstname << " " << student.lastname
       << " (Year: " << student.year << ", Grade: " << student.avg_grade << ")";
    return os;
  }

  friend istream &operator>>(istream &is, Student &student) {
    string firstname, lastname;
    int year;
    double grade;

    if (is >> firstname >> lastname >> year >> grade) {
      student.set_firstname(firstname);
      student.set_lastname(lastname);
      student.set_year(year);
      student.set_grade(grade);
    }

    return is;
  }
};
