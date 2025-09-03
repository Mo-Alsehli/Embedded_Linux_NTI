#include <my_array.h>

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// Inheritance.
class Employee {
   public:
    std::string name;
    int salary;
    int age;
    int years_of_experience;

    Employee(std::string name){

    };

   protected:
    int id;
};

class Engineer : public Employee {
    Engineer(std::string name) : Employee(name) { id = 5555; };

   private:
    std::string specialization;
    std::string programming_language;
    std::string team;
};

int main() {
    // Array my_arr(10);
    // my_arr.write(0, 10);
    // my_arr.write(1, 20);
    // my_arr.write(2, 30);
    // my_arr.write(3, 40);
    // my_arr.write(4, 50);
    // my_arr.write(15, 60);

    // std::cout << my_arr.read(1) << std::endl;
    // std::cout << my_arr.read(2) << std::endl;
    // std::cout << my_arr.read(3) << std::endl;
    // std::cout << my_arr.read(15) << std::endl;
    // =====================================================================================
    // Testing my custom assignment operator overloading.
    // Array<int> a(10);
    // Array<int> b(5);

    // a.write(0, 1);
    // a.write(1, 2);
    // a.write(2, 3);
    // a.write(3, 4);
    // a.write(4, 5);
    // a.write(5, 6);
    // a.write(6, 7);

    // a[6] = 20;

    // std::cout << "Index 2 value in Array(a): " << a.read(2) << std::endl;
    // std::cout << "Index 3 value in Array(a): " << a.read(3) << std::endl;

    // b = a;

    // std::cout << "Index 2 value in Array(b): " << b.read(2) << std::endl;
    // std::cout << "Index 3 value in Array(b): " << b.read(3) << std::endl;
    // std::cout << "Index 6 value in Array(b): " << b.read(6) << std::endl;

    // ========================================================================================

    // std::vector<int> my_v(10);
    // int num = 10;
    // for (int i = 9; i >= 0; i--) {
    //     my_v[i] = num;
    //     num *= 2;
    // }

    // std::cout << "Vecotr Before Sorting" << std::endl;
    // std::cout << "to_vector contains: ";
    // std::copy(my_v.begin(), my_v.end(), std::ostream_iterator<int>(std::cout, " "));
    // std::cout << '\n';

    // std::vector<int>::iterator it = my_v.begin();

    // for (; it != my_v.end(); it++) {
    //     bool swapped = false;
    //     std::vector<int>::iterator j = my_v.begin();
    //     while (j != (my_v.end() - 1)) {
    //         if (*j > *(j + 1)) {
    //             int temp = *j;
    //             *j = *(j + 1);
    //             *(j + 1) = temp;
    //             swapped = true;
    //         }
    //         ++j;
    //     }
    //     if (!swapped) break;
    // }

    // std::cout << "Vecotr AFter Sorting" << std::endl;
    // std::cout << "to_vector contains: ";
    // std::copy(my_v.begin(), my_v.end(), std::ostream_iterator<int>(std::cout, " "));
    // std::cout << '\n';

    // ========================================================================================

    return 0;
}