#include <iostream>
#include <vector>
#include <string>
#include "exprtk.hpp"

#define WITHOUT_NUMPY
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

double strtod (std::string a) { return atof(a.c_str()); }

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


template <typename T>
double make_function(std::string funct, double x_val, std::string x_str) {
    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T> expression_t;
    typedef exprtk::parser<T> parser_t;
    
    std::string expr_string = funct;
    
    const T t = T(x_val);
    
    symbol_table_t symbol_table;
    symbol_table.add_constant(x_str,t);
    
    expression_t expression;
    expression.register_symbol_table(symbol_table);
    
    parser_t parser;
    parser.compile(expr_string, expression);
    
    return expression.value();
}

//creates lists of x and f(x)
std::vector<std::vector<double>> createVectors(std::string f, double x_min, double x_max, double h) {
    int n = (int) ((x_max - x_min) / h);
    std::vector<double> x_vals,y_vals;
    double x_i = x_min;
    double y_i = make_function<double>(f,x_min,"x");
    for (int i = 0; i < n; i++) {
        std::cout << "at x = " << x_i << " y = " << y_i << std::endl;
        x_vals.push_back(x_i);
        y_vals.push_back(y_i);
        
        x_i = x_i + h;
        y_i = make_function<double>(f,x_i,"x");
    }
    std::cout << "at x = " << x_i << " y = " << y_i << std::endl;
    x_vals.push_back(x_i);
    y_vals.push_back(y_i);
    std::vector<std::vector<double>> ret;
    ret.push_back(x_vals);
    ret.push_back(y_vals);
    return ret;
}

int main(int argc, char** argv) {

    std::cout << "\nWelcome to a Grapher\n";
    std::cout << "Enter y = f(x): ";
    std::string f; std::getline(std::cin, f);
    std::cout << "minimum x: ";
    std::string x_min_str; std::getline(std::cin, x_min_str); double x_min = strtod(x_min_str);
    std::cout << "maximum x: ";
    std::string x_max_str; std::getline(std::cin, x_max_str); double x_max = strtod(x_max_str);
    std::cout << "step size: ";
    std::string h_str; std::getline(std::cin, h_str); double h = strtod(h_str);
    std::vector<std::vector<double>> result = createVectors(f,x_min,x_max,h);
    std::vector<double> x_vals = result.at(0); std::vector<double> y_vals = result.at(1);
    print_vector(x_vals); print_vector(y_vals);
    plt::plot(x_vals, y_vals);
    plt::show();
    


    return 0;
}
