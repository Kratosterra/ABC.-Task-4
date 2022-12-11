#include <pthread.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;

// Портфель
int briefcase = 0;
// Верхняя граница работы
int deadline = 0;

/// @brief Функция, получающая тип работы программы, анализируя входные данные из командной строки.
/// @param argc количество аругументов
/// @param argv аргументы
/// @return код типа обработки
int get_type_of_work(int argc, char **argv)
{
    // По колличеству аргументов выбираем тип работы
    if (argc == 2)
    {
        // Проверим файл вывода на доступность
        cout << "Производим генерацию!\n";
        ifstream iff(argv[1]);
        if (!iff.is_open())
        {
            cout << "Внимание: Файл вывода не обнаружен или недоступен!\n";
            return -1;
        }
        iff.close();
        return 2;
    }
    else if (argc == 3)
    {
        cout << "Производим cчитывание из файлов!\n";
        // Проверяем входной файл на доступность
        ifstream iff(argv[1]);
        if (!iff.is_open())
        {
            cout << "Файл ввода не обнаружен или недоступен!\n";
            return -1;
        }
        iff.close();
        // Проверяем выходной файл на доступность
        ifstream ifd(argv[2]);
        if (!ifd.is_open())
        {
            cout << "Внимание: Файл вывода не обнаружен или недоступен!";
            return -1;
        }
        ifd.close();
        return 1;
    }
    else if (argc == 1)
    {
        cout << "Начинаем работу в консольном режиме!\n";
        return 0;
    }
    else if (argc == 4)
    {
        cout << "Начинаем работу в режиме командной строки!\n";
        return 3;
    }
    else
    {
        puts("Неверное количество параметров.");
        return -1;
    }
}

/// @brief Функция, получающая входные данные из консоли.
/// @param data ссылка на входные данные
void get_data(vector<int> &data)
{
    cout << "Введите колличество рядов, шкафов в каждом ряду и книг в каждом шкафу:\n";
    cin >> data[0] >> data[1] >> data[2];
}

/// @brief Функция, получающая входные данные из файла.
/// @param data ссылка на входные данные
void get_data_file(vector<int> &data, char **argv)
{
    ifstream iff(argv[1]);
    iff >> data[0] >> data[1] >> data[2];
    iff.close();
}

/// @brief Функция, производящая генерацию входных данных.
/// @param data ссылка на входные данные
void generate_data(vector<int> &data)
{
    srand(static_cast<unsigned int>(time(0)));
    data[0] = 1 + rand() % 100;
    data[1] = 1 + rand() % 100;
    data[2] = 1 + rand() % 100;
}

/// @brief Структура книги
struct Book
{
    // Название книги
    std::string name;
    // Имя студента, добавившего книгу
    std::string name_of_tread;
    // Номер ряда
    int M = 0;
    // Номер шкафа
    int N = 0;
    // Номер книги в шкафу
    int K = 0;
};

/// @brief Стркутура аргумента.
struct Arguments
{
    // Задачи
    std::vector<std::vector<int>> tasks;
    // Каталог книг
    std::vector<Book> catalog;
    // Текущее состояние портфеля
    int briefcase;
    // Максимальное колличество задач
    int deadline;
    // Аргументы командной строоки
    char **argv;
};

/// @brief Функция, которая печатает каталог
/// @param catalog каталог книг
/// @param choice тип работы программы
/// @param argv аргументы командной строки
void print_catalog(const vector<Book> &catalog, int choice, char **argv)
{
    if (choice == 0 || choice == 3)
    {
        // Печатаем информацию в консоль
        for (const auto &i : catalog)
        {
            cout << "Книга: " << i.name << " Студент: " << i.name_of_tread << " Номер ряда: "
                 << i.M << " Номер шкафа: " << i.N << " Номер книги: " << i.K << "\n";
        }
    }
    else if (choice == 1)
    {
        // Печтаем информацию в выходной файл и консоль
        std::ofstream out;
        out.open(argv[2]);
        if (out.is_open())
        {
            for (const auto &i : catalog)
            {
                out << "Книга: " << i.name << " Студент: " << i.name_of_tread << " Номер ряда: "
                    << i.M << " Номер шкафа: " << i.N << " Номер книги: " << i.K << "\n";
                cout << "Книга: " << i.name << " Студент: " << i.name_of_tread << " Номер ряда: "
                     << i.M << " Номер шкафа: " << i.N << " Номер книги: " << i.K << "\n";
            }
        }
        cout << "Запись ответа в файл завершена!\n";
        out.close();
    }
    else if (choice == 2)
    {
        // Печтаем информацию в выходной файл и консоль
        std::ofstream out;
        out.open(argv[1]);
        if (out.is_open())
        {
            for (const auto &i : catalog)
            {
                out << "Книга: " << i.name << " Студент: " << i.name_of_tread << " Номер ряда: "
                    << i.M << " Номер шкафа: " << i.N << " Номер книги: " << i.K << "\n";
                cout << "Книга: " << i.name << " Студент: " << i.name_of_tread << " Номер ряда: "
                     << i.M << " Номер шкафа: " << i.N << " Номер книги: " << i.K << "\n";
            }
        }
        cout << "Запись ответа в файл завершена!\n";
        out.close();
    }
}

// Мьютекс портфеля задач
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
// Мьютекс вывода информации о потоке и его работе.
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

/// @brief Функция заполнения каталога книгами
/// @param ptr структура аргумента
/// @return nullptr и завершает поток
void *put_book_catalog(void *ptr)
{
    // Имя потока
    char name[20];
    // Координаты книги
    vector<int> task;
    // Получаем аргументы
    auto *args = (Arguments *)ptr;
    while (true)
    {
        // Закрываем мьютекс портфеля
        pthread_mutex_lock(&mutex1);
        // Повышаем номер задачи в портфеле
        args->briefcase += 1;
        // Открываем мьютекс портфеля
        pthread_mutex_unlock(&mutex1);
        // Получаем номер в каталоге
        int num = args->briefcase - 1;
        // Если максимальное колличество задач не достигнуто - работаем, иначе завершаем поток
        if (num < args->deadline)
        {
            // Получаем координаты
            task = args->tasks[num];
            // Получаем имя  потока
            pthread_getname_np(pthread_self(), name, 20);
            // Проверяем главный ли это поток
            string check = string(name);
            check = "./" + check;
            // Если это главный поток - это работает Староста
            // В другом случае работает поток одного из студентов
            if (check == args->argv[0])
            {
                // Добавляем книгу в каталог
                args->catalog[num] = Book{to_string(num + 1), "Староста", task[0] + 1, task[1] + 1, task[2] + 1};
            }
            else
            {
                // Добавляем книгу в каталог
                args->catalog[num] = Book{to_string(num + 1), name, task[0] + 1, task[1] + 1, task[2] + 1};
            }
        }
        else
        {
            return (nullptr);
        }

        // Выводим данные о работе в консоль
        string check = string(name);
        check = "./" + check;
        // Закрываем мьютекс вывода
        pthread_mutex_lock(&mutex2);
        if (check == args->argv[0])
        {
            std::cout << "Cтудент Cтароста"
                      << " внес книгу \"" << to_string(num + 1)
                      << "\" в каталог по месту (ряд: " << task[0] + 1 << " шкаф: "
                      << task[1] + 1 << " книга: " << task[2] + 1 << ")\n";
        }
        else
        {
            std::cout << "Cтудент " << name << " внес книгу \"" << to_string(num + 1)
                      << "\" в каталог по месту (ряд: " << task[0] + 1 << " шкаф: "
                      << task[1] + 1 << " книга: " << task[2] + 1 << ")\n";
        }
        // Открываем мьютекс вывода
        pthread_mutex_unlock(&mutex2);
    }
}

/// @brief Функция, сторящая на основе входных данных список задач.
/// @param data_M_N_K входные данные
/// @return Вектор координат книг.
vector<vector<int>> get_tasks(const vector<int> &data_M_N_K)
{
    // Создаем вектор задач.
    vector<vector<int>> tasks;
    for (int m = 0; m < data_M_N_K[0]; m++)
    {
        for (int n = 0; n < data_M_N_K[1]; n++)
        {
            for (int k = 0; k < data_M_N_K[2]; k++)
            {
                // В векторе будут представлены векторы, содержащие координаты книги.
                vector<int> temp = {m, n, k};
                tasks.push_back(temp);
            }
        }
    }
    return tasks;
}

/// @brief Главная функция программы
/// @param argc количество аргументов
/// @param argv аргументы
/// @return код завршения
int main(int argc, char **argv)
{
    // Вектор для входных данных
    auto data_M_N_K = std::vector<int>(3);
    // Число представляющее собой тип ввода данных
    int choice = get_type_of_work(argc, argv);
    if (choice == 0)
    {
        // Получаем входные даннные из консоли
        get_data(data_M_N_K);
    }
    else if (choice == 1)
    {
        // Получаем входные данные из файла
        get_data_file(data_M_N_K, argv);
    }
    else if (choice == 2)
    {
        // Генерируем входные данные
        generate_data(data_M_N_K);
    }
    else if (choice == 3)
    {
        data_M_N_K[0] = atoi(argv[1]);
        data_M_N_K[1] = atoi(argv[2]);
        data_M_N_K[2] = atoi(argv[3]);
    }
    else
    {
        // Сигнализируем об ошибке
        cout << "Произошла ошибка исполнения!\n";
        return 0;
    }
    // Выводим входные данные на экран
    cout << "----------------\nВведеные данные:\nM: " << data_M_N_K[0] << " N: " << data_M_N_K[1]
         << " K: " << data_M_N_K[2] << "\n----------------\n\n";
    // Проверяем корректнность данных
    if (data_M_N_K[0] < 0 || data_M_N_K[1] < 0 || data_M_N_K[2] < 0)
    {
        // Сигнализируем об ошибке
        cout << "Произошла ошибка исполнения! Не может быть отрицательного числа!\n";
        return 0;
    }
    // Подсчитываем максимальное колличество задач
    deadline = data_M_N_K[0] * data_M_N_K[1] * data_M_N_K[2];
    // Создаем задачи в виде координат книги
    vector<vector<int>> tasks = get_tasks(data_M_N_K);
    // Cоздаем вектор книг размера колличества задач
    auto catalog = vector<Book>(deadline);
    // Создаем структуру аргумента для передачи потокам
    auto *arguments = new Arguments{tasks, catalog, briefcase, deadline, argv};
    cout << "-Информация о работе-------------------------------\n";
    // Cоздаем 4 потока студентов
    pthread_t thread_vanya, thread_oleg, thread_sasha, thread_dima;
    // Каждый студент получает на исполнение функцию и аргументы для работы
    pthread_create(&thread_vanya, nullptr, put_book_catalog, (void *)arguments);
    pthread_create(&thread_oleg, nullptr, put_book_catalog, (void *)arguments);
    pthread_create(&thread_sasha, nullptr, put_book_catalog, (void *)arguments);
    pthread_create(&thread_dima, nullptr, put_book_catalog, (void *)arguments);
    // Даем им имена
    pthread_setname_np(thread_dima, "Димочка");
    pthread_setname_np(thread_vanya, "Ванечка");
    pthread_setname_np(thread_oleg, "Олежа");
    pthread_setname_np(thread_sasha, "Санёк");
    // Подключем их к главному потоку программы
    pthread_join(thread_oleg, nullptr);
    pthread_join(thread_sasha, nullptr);
    pthread_join(thread_vanya, nullptr);
    pthread_join(thread_dima, nullptr);
    cout << "-Конец информации о работе-------------------------------\n";
    // Выводим каталог на экран и записывает в выходной файл, если нужно.
    print_catalog(arguments->catalog, choice, argv);
    // Удаляем аргументы.
    delete arguments;
    return 0;
}