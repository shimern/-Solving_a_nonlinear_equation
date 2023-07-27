﻿#include <iostream>
#include <cmath>
#include <omp.h>
#include <chrono>
#include <iomanip>

#define threadsNum 40

using namespace std;

double f(double x) {
    return cos(x)+2*x;
}

double integral(double a, double b, double accuracy) {
    double h;
    int n;
    double s[2]{};
    double delta = accuracy + 1;
    int count = 0;

    // КФ метод Симпсона 3/8
    for (int i = 1; abs(delta) >= accuracy; i++) {
        n = 3 * i;
        h = (b - a) / n; // шаг

        s[1] = 0;
        double x = a + h;

        omp_set_num_threads(threadsNum);
        #pragma omp parallel for reduction(+:s[1])
        for (int i = 0; x <= b; i++)
        {
            s[1] += f(x - h) + 3 * f((2 * (x - h) + x) / 3)+ 3 * f((x - h + 2 * x) / 3) + f(x);
            x = a+i*h;
        }
        s[1] *= h / 8;
       /* while (x <= b) {
            s[1] += f(x - h) + 3 * f((2 * (x - h) + x) / 3) + 3 * f((x - h + 2 * x) / 3) + f(x);
            x += h;
        }
        s[1] *= h / 8;*/

        //omp_set_num_threads(threadsNum);
        //#pragma omp parallel for reduction(+:s[1])
        //for (int j = 1; j <= n - 1; j++) {
        //    double xj = a + j * h;
        //    if (j % 3 == 0) {
        //        s[1] += 2 * f(xj);
        //    }
        //    else {
        //        s[1] += 3 * f(xj);
        //    }
        //}
        //s[1] = (3 * h / 8) * (s[1] + f(a) + f(b));


        delta = abs(s[1] - s[0]);
        s[0] = s[1];
        count = i;
    }
    return s[1]; // находим значение в точке F_b
}

int main() {
    setlocale(LC_CTYPE, "ru");

    double a, b;
    cout << "Введите коэффициенты a, b: ";
    cin >> a >> b;

    double accuracy;
    cout << "Введите точность измерения: ";
    cin >> accuracy;

    double x0, x1 = 0;
    cout << "Введите начальное приближение: ";
    cin >> x0;

    auto start = chrono::high_resolution_clock::now();

    double delta = accuracy + 1; // нужно, чтобы проверять на точность

    double F1, F2;
    double h = 3 * accuracy / 10; // чтоб шаг был кратен 3

    while (abs(delta) >= accuracy) {
        F1 = integral(a, x0, accuracy)-b;
        F2 = integral(a, x0 + h, accuracy)-b;
        x1 = x0 - F1 * h / (F2 - F1);
        delta = x1 - x0;

        x0 = x1;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Количество потоков: " << threadsNum << endl;
    cout << "Время выполнения: " << duration.count() << " секунд" << endl;
    cout <<"Ответ:"<< x1 << endl;

    return 0;
}