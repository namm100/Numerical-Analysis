#include <iostream>
#include <vector>
#include <string>
#include "exprtk.hpp"

#define WITHOUT_NUMPY
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

void print_vector(std::vector<double> a) {
    if (a.size() == 1) {
        std::cout << "\n(" << a.at(0) << ")\n";
        return;
    }

    std::cout << "\n(";
    for (int i = 0; i < a.size() - 1; i++) {
        std::cout << a.at(i) << ", ";
    }
    std::cout << a.at(a.size() - 1) << ")\n";
}

double strtod (std::string a) {
    return atof(a.c_str());
}

template <typename T>
double make_function(std::string funct,double t_val,double y_val) {
    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T> expression_t;
    typedef exprtk::parser<T> parser_t;
    
    std::string expr_string = funct;
    
    const T t = T(t_val);
    const T y = T(y_val);
    
    symbol_table_t symbol_table;
    symbol_table.add_constant("t",t);
    symbol_table.add_constant("y",y);
    
    expression_t expression;
    expression.register_symbol_table(symbol_table);
    
    parser_t parser;
    parser.compile(expr_string, expression);
    
    return expression.value();

}

double f(std::string funct, double t, double y) {
    return make_function<double>(funct, t, y);
}

// differential equation in dy/dt = f(y,t)
std::vector<std::vector<double>> rk4(std::string funct, double y_0, double t_0, double h,int n) {
    int i = 0; // counter
    double t_i = t_0;
    double y_i = y_0;
    std::vector<double> t_vect;
    std::vector<double> y_vect;

    for (int i = 0; i < n; i++) {
        double k1 = f(funct, t_i, y_i);
        double k2 = f(funct, t_i + h /2, y_i + h*k1 / 2);
        double k3 = f(funct, t_i + h /2, y_i + h*k2 /2);
        double k4 = f(funct, t_i + h, y_i + h*k3);
        std::cout << "at t = " << t_i << " y = " << y_i << std::endl;
        t_vect.push_back(t_i);
        y_vect.push_back(y_i);
        t_i = t_i + h;
        y_i = y_i + h * (k1 + 2*k2 + 2*k3 + k4) / 6;
    }
    std::cout << "at t = " << t_i << " y = " << y_i << std::endl;
    t_vect.push_back(t_i);
    y_vect.push_back(y_i);
    std::vector<std::vector<double>> ret;
    ret.push_back(t_vect);
    ret.push_back(y_vect);
    return ret;
}

void euler(std::string funct, double y_0, double t_0, double h, int n) {
    
    int i = 0; // counter 
    double t_i = t_0;
    double y_i = y_0;
    for (int i = 0; i < n; i++) {
        std::cout << "at t = " << t_i << " y = " << y_i << std::endl;
        t_i = t_i + h;
        y_i = y_i + h * f(funct, t_i, y_i);
    }
    std::cout << "at t = " << t_i << " y = " << y_i << std::endl;

}

// consider a differential equations y'' = f(t,y,y'), y(t_0)=y_0, y'(t_0) = y'_0
// solve:
// dv/dt = f(t,y,v), v(t_0) = y'_0
// then:
// dy/dt = v, y(t_0) = y_0

template <typename T>
double make_function_3(std::string funct, double t_val, double y_val, double y_prime_val, std::string t_str,std::string y_str, std::string y_prime_str) {
    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T> expression_t;
    typedef exprtk::parser<T> parser_t;
    
    std::string expr_string = funct;
    
    const T t = T(t_val);
    const T y = T(y_val);
    const T y_prime = T(y_prime_val);
    
    symbol_table_t symbol_table;
    symbol_table.add_constant(t_str, t); // t, x
    symbol_table.add_constant(y_str, y); // y, y
    symbol_table.add_constant(y_prime_str, y_prime); // p, z
    
    expression_t expression;
    expression.register_symbol_table(symbol_table);
    
    parser_t parser;
    parser.compile(expr_string, expression);
    return expression.value();
}


std::vector<std::vector<double>> second_order_euler(std::string f, double t_0, double y_0, double y_prime_0, double h, double n) {
    
    // z_k denotes approximation to y(t_k)
    // w_k denotes approximation to v(t_k)
    // v' = f(t,y,v), v(t_0) = y'_0
    // y' = v, y(t_0) = y_0
    // z_k+1 = z_k + (t_k+1 - t_k)*v_k
    // v_k+1 = v_k + (t_k+1 - t_k)*f(t,z_k,v_k)
    
    int i = 0; // counteer
    std::vector<double> t_vect, y_vect;
    double t_i = t_0;
    double z_i = y_0;
    double w_i = y_prime_0;
    
    for (int i = 0; i < n; i++) {
        std::cout << "at t = " << t_i << " y = " << z_i << std::endl;
        t_vect.push_back(t_i);
        y_vect.push_back(z_i);
        
        t_i = t_i + h;
        z_i = z_i + h * w_i;
        w_i = w_i + h * make_function_3<double>(f,t_i,z_i,w_i,"t","y","p");
    }
    std::cout << "at t = " << t_i << " y = " << z_i << std::endl;
    t_vect.push_back(t_i);
    y_vect.push_back(z_i);
    std::vector<std::vector<double>> ret;
    ret.push_back(t_vect);
    ret.push_back(y_vect);
    return ret;
}
std::vector<std::vector<double>> two_first_order_rk4(std::string f, std::string g, double x_0, double y_0, double z_0, double h, double n) {
    
    // this solves dy/dx = f(x,y,z) and dz/dx = g(x,y,z) with z(x_0) = z_0 and y(x_0) = y_0
    
    int i = 0;
    std::vector<double> x_vect,y_vect,z_vect;
    double x_i = x_0;
    double y_i = y_0;
    double z_i = z_0;
    
    for (int i = 0; i < n; i++) {
        std::cout << "at x = " << x_i << " y = " << y_i << " z = " << z_i << std::endl;
        x_vect.push_back(x_i);
        y_vect.push_back(y_i);
        z_vect.push_back(z_i);
        
        double k_0 = h * make_function_3<double>(f,x_i,y_i,z_i,"x","y","z");
        double l_0 = h * make_function_3<double>(g,x_i,y_i,z_i,"x","y","z");
        
        double k_1 = h * make_function_3<double>(f,x_i + (0.5)*h,y_i + (0.5)*k_0,z_i + (0.5)*l_0,"x","y","z");
        double l_1 = h * make_function_3<double>(g,x_i + (0.5)*h,y_i + (0.5)*k_0,z_i + (0.5)*l_0,"x","y","z");
        
        double k_2 = h * make_function_3<double>(f,x_i + (0.5)*h,y_i + (0.5)*k_1,z_i + (0.5)*l_1,"x","y","z");
        double l_2 = h * make_function_3<double>(g,x_i + (0.5)*h,y_i + (0.5)*k_1,z_i + (0.5)*l_1,"x","y","z");
        
        double k_3 = h * make_function_3<double>(f,x_i + h, y_i + k_2, z_i + l_2, "x","y","z");
        double l_3 = h * make_function_3<double>(g,x_i + h, y_i + k_2, z_i + l_2, "x","y","z");
        
        //std::cout << "k0: " << k_0 << " k1: " << k_1 << " k2: " << k_2 << " k3: " << k_3 << std::endl;
        //std::cout << "l0: " << l_0 << " l1: " << l_1 << " l2: " << l_2 << " l3: " << l_3 << std::endl;
        
        y_i = y_i + (1.0/6.0)*(k_0 + 2.0*k_1 + 2.0*k_2 + k_3);
        z_i = z_i + (1.0/6.0)*(l_0 + 2.0*l_1 + 2.0*l_2 + l_3);
        x_i = x_i + h;
    }
    std::cout << "at x = " << x_i << " y = " << y_i << " z = " << z_i << std::endl;
    x_vect.push_back(x_i);
    y_vect.push_back(y_i);
    z_vect.push_back(z_i);
    std::vector<std::vector<double>> ret;
    ret.push_back(x_vect);
    ret.push_back(y_vect);
    ret.push_back(z_vect);
    return ret;
}

void startTwoFirstOrder() {
    
    std::cout << "\nCoupled First Order Differential Equation Solver\n";
    std::cout << "\ndy/dx = f(x,y,z) and dz/dx = g(x,y,z)\n";
    std::cout << "Input function f(x,y,z): ";
    std::string f_funct; std::getline(std::cin, f_funct);
    std::cout << "Input function g(x,y,z): ";
    std::string g_funct; std::getline(std::cin, g_funct);
    std::cout << "Input inital x: ";
    std::string x_0_str; std::getline(std::cin, x_0_str);
    double x_0 = strtod(x_0_str);
    std::cout << "Input y(x_0): ";
    std::string y_0_str; std::getline(std::cin, y_0_str);
    double y_0 = strtod(y_0_str);
    std::cout << "Input z(x_0): ";
    std::string z_0_str; std::getline(std::cin, z_0_str);
    double z_0 = strtod(z_0_str);
    std::cout << "Input step size (h): ";
    std::string h_str; std::getline(std::cin, h_str);
    double h = strtod(h_str);
    std::cout << "Input number of iterations (n): ";
    std::string n_str; std::getline(std::cin, n_str);
    double n = strtod(n_str);
    
    std::vector<std::vector<double>> result = two_first_order_rk4(f_funct,g_funct,x_0,y_0,z_0,h,n);
    std::vector<double> x_vals = result.at(0);
    std::vector<double> y_vals = result.at(1);
    std::vector<double> z_vals = result.at(2);
    
    print_vector(y_vals);
    print_vector(z_vals);
    
    plt::plot(x_vals, y_vals);
    plt::plot(x_vals, z_vals);
    plt::show();
}


void startSecondOrderODESolver() {
    
    std::cout << "\nSecond Order Differential Equation Solver\n";
    std::cout << "In the form d2y/dt2 = f(t,y,y')";
    std::cout << "Input function f(t,y,y'=p) HOWEVER, USE p in place of y': ";
    std::string funct;
    std::getline(std::cin, funct);
    std::cout << "Input starting time: ";
    std::string t_0_str;
    std::getline(std::cin, t_0_str);
    std::cout << "Input initial y: ";
    std::string y_0_str;
    std::getline(std::cin, y_0_str);
    std::cout << "Input initial y': ";
    std::string y_0_prime_str;
    std::getline(std::cin, y_0_prime_str);
    std::cout << "Input step size: ";
    std::string h_str;
    std::getline(std::cin, h_str);
    std::cout << "Input number of iterations: ";
    std::string n_str;
    std::getline(std::cin, n_str);
    double t_0 = strtod(t_0_str);
    double y_0 = strtod(y_0_str);
    double y_0_prime = strtod(y_0_prime_str);
    double h = strtod(h_str);
    double n = strtod(n_str);
    
    std::vector<std::vector<double>> second_order_euler_result = second_order_euler(funct, t_0, y_0, y_0_prime, h, n);
    std::cout << "\nEuler's Method:" << std::endl;
    std::vector<double> t_values_sec_euler = second_order_euler_result.at(0);
    std::vector<double> y_values_sec_euler = second_order_euler_result.at(1);
    print_vector(t_values_sec_euler);
    print_vector(y_values_sec_euler);
    
    plt::plot(t_values_sec_euler, y_values_sec_euler);
    plt::show();
    
}

int main (int argc, char** argv) {
    
    std::cout << "Welcome to an ordinary differential equation solver" << std::endl;
    std::cout << "First order (1) or second order (2) or two first order (3): ";
    std::string response;
    std::getline(std::cin, response);
    if (strtod(response) == 2) {
        startSecondOrderODESolver();
        return 0;
    }
    if (strtod(response) == 3) {
        startTwoFirstOrder();
        return 0;
    }
    
    std::cout << "\nFirst Order Differential Equation Solver\n";
    std::cout << "In the form dy/dt = f(t,y)\n";
    std::cout << "Input function f(t,y): ";
    std::string funct;
    std::getline(std::cin, funct);   
    std::cout << "Input starting time: ";
    std::string t_0_str;
    std::getline(std::cin,t_0_str);
    std::cout << "Input starting value: ";
    std::string y_0_str;
    std::getline(std::cin, y_0_str);
    std::cout << "Input step size: ";
    std::string h_str;
    std::getline(std::cin, h_str);
    std::cout << "Input number of iterations: ";
    std::string n_str;
    std::getline(std::cin, n_str);
    double y_0 = strtod(y_0_str);
    double t_0 = strtod(t_0_str);
    double h = strtod(h_str);
    double n = strtod(n_str);    
    
    std::vector<std::vector<double>> rk4_result;
    std::cout << "\nRK4 Method:" << std::endl; 
    rk4_result = rk4(funct, y_0, t_0, h, n);
    std::vector<double> t_values_rk4 = rk4_result.at(0);
    std::vector<double> y_values_rk4 = rk4_result.at(1);
    print_vector(t_values_rk4);
    print_vector(y_values_rk4);


    //std::cout << "\nEuler's Method:" << std::endl;
    //euler(funct, y_0, t_0, h, n);

    // plotting RK4
    plt::plot(t_values_rk4, y_values_rk4);
    plt::show();
    return 0;
}
