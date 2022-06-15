#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// Перевод списка смежности в матрицу смежности.
[[maybe_unused]] vector<vector<int>> FromAdjacencyListToAdjacencyMatrix(size_t count_of_vertexes,
                                                                        vector<vector<int>> &adjacency_list) {
  vector<vector<int>> adjacency_matrix(count_of_vertexes, vector<int>(count_of_vertexes, 0));
  for (size_t i = 0; i < count_of_vertexes; ++i) {
    for (auto vertex : adjacency_list[i]) {
      adjacency_matrix[i][vertex] = 1;
    }
  }
  return adjacency_matrix;
}

// Перевод списка ребер в матрицу смежности.
vector<vector<int>> FromEdgesListToAdjacencyMatrix(size_t count_of_vertexes,
                                                   vector<pair<int, int>> &list_of_edges,
                                                   bool is_orient) {
  vector<vector<int>> adjacency_matrix(count_of_vertexes, vector<int>(count_of_vertexes, 0));
  for (auto edge : list_of_edges) {
    adjacency_matrix[edge.first - 1][edge.second - 1] = 1;
    if (!is_orient) {
      adjacency_matrix[edge.second - 1][edge.first - 1] = 1;
    }
  }
  return adjacency_matrix;
}

// Перевод матрицы инцидентности в матрицу смежности.
vector<vector<int>> FromIncidenceMatrixToAdjacencyMatrix(size_t count_of_vertexes,
                                                         size_t count_of_edges,
                                                         vector<vector<int>> incidence_matrix,
                                                         bool is_orient) {
  vector<vector<int>> adjacency_matrix(count_of_vertexes, vector<int>(count_of_vertexes, 0));
  for (size_t i = 0; i < count_of_edges; ++i) {
    vector<int> edge;
    int from = -2;
    int to = -2;
    for (size_t j = 0; j < count_of_vertexes; ++j) {
      if (incidence_matrix[j][i] == 0)
        continue;
      if (is_orient) {
        if (incidence_matrix[j][i] == 1)
          to = static_cast<int>(j);
        else
          from = static_cast<int>(j);
      } else {
        if (from == -2)
          from = static_cast<int>(j);
        else
          to = static_cast<int>(j);
      }
    }
    if (from != -2 && to != -2) {
      adjacency_matrix[from][to] = 1;
      if (!is_orient)
        adjacency_matrix[to][from] = 1;
    }
  }
  return adjacency_matrix;
}

// Класс, реализующий работу с графами.
class Graph {
 private:
  // Количество ввершин
  size_t count_of_vertexes_;
  size_t count_of_edges_;
  bool is_orient_;
  string output_path_;
  string input_path_;
  vector<vector<int>> adjacency_matrix_;

  // Перевод матрицы смежности в матрицу инцидентности.
  vector<vector<int>> GetIncidenceMatrix(size_t count_of_vertexes,
                                         size_t count_of_edges,
                                         const vector<vector<int>> &adjacency_matrix,
                                         bool is_orient) {
    vector<vector<int>> incidence_matrix(count_of_vertexes, vector<int>(count_of_edges, 0));
    vector<pair<int, int>> list_of_edges = GetListOfEdges(count_of_vertexes, adjacency_matrix, is_orient);
    for (size_t i = 0; i < list_of_edges.size(); ++i) {
      int vertex_from = list_of_edges[i].first - 1;
      int vertex_to = list_of_edges[i].second - 1;
      incidence_matrix[vertex_from][i] = is_orient ? -1 : 1;
      incidence_matrix[vertex_to][i] = 1;
    }
    return incidence_matrix;
  }

  // Перевод матрицы смежности в список ребер.
  vector<vector<int>> GetAdjacencyList(size_t count_of_vertexes, vector<vector<int>> adjacency_matrix) {
    vector<vector<int>> adjacency_list(count_of_vertexes);
    for (size_t i = 0; i < count_of_vertexes; ++i) {
      for (size_t j = 0; j < count_of_vertexes; ++j) {
        if (adjacency_matrix[i][j]) {
          adjacency_list[i].push_back(static_cast<int>(j));
        }
      }
    }
    return adjacency_list;
  }

  // Перевод матрицы смежности в список ребер.
  vector<pair<int, int>> GetListOfEdges(size_t count_of_vertexes,
                                        vector<vector<int>> adjacency_matrix,
                                        bool is_orient) {
    vector<pair<int, int>> list_of_edges;
    for (size_t i = 0; i < count_of_vertexes; ++i) {
      for (size_t j = 0; j < count_of_vertexes; ++j) {
        if (!is_orient && i >= j)
          continue;
        if (adjacency_matrix[i][j]) {
          list_of_edges.emplace_back(i + 1, j + 1);
        }
      }
    }
    return list_of_edges;
  }

  // Рекурсивный обход графа dfs.
  void RecursionDfs(vector<bool> &is_used_vertex, size_t current_vertex, vector<int> &current_vertexes) {
    is_used_vertex[current_vertex] = true;
    current_vertexes.push_back(static_cast<int>(current_vertex));
    for (size_t i = 0; i < count_of_vertexes_; ++i)
      if (adjacency_matrix_[current_vertex][i] && !is_used_vertex[i])
        RecursionDfs(is_used_vertex, i, current_vertexes);
  }

  // Нерекурсивный обход графа dfs.
  void StackDfs(vector<bool> &is_used_vertex, size_t start_vertex, vector<int> &current_vertexes) {
    current_vertexes.push_back(static_cast<int>(start_vertex));
    is_used_vertex[start_vertex] = true;
    stack<size_t> vertexes_stack;
    vertexes_stack.push(start_vertex);
    while (!vertexes_stack.empty()) {
      bool was_add = false;
      size_t current_vertex = vertexes_stack.top();
      is_used_vertex[current_vertex] = true;
      for (size_t i = 0; i < count_of_vertexes_; ++i) {
        if (adjacency_matrix_[current_vertex][i] && !is_used_vertex[i]) {
          current_vertexes.push_back(static_cast<int>(i));
          vertexes_stack.push(i);
          was_add = true;
          break;
        }
      }
      if (was_add)
        continue;
      vertexes_stack.pop();
    }
  }

  // Обход графа bfs.
  void Bfs(size_t start_vertex, vector<bool> &is_used_vertex, vector<int> &current_vertexes) {
    queue<size_t> vertexes_queue;
    vertexes_queue.push(start_vertex);
    while (!vertexes_queue.empty()) {
      size_t current_vertex = vertexes_queue.front();
      vertexes_queue.pop();
      if (is_used_vertex[current_vertex])
        continue;
      current_vertexes.push_back(static_cast<int>(current_vertex));
      is_used_vertex[current_vertex] = true;
      for (size_t i = 0; i < count_of_vertexes_; ++i) {
        if (adjacency_matrix_[current_vertex][i] && !is_used_vertex[i]) {
          vertexes_queue.push(i);
        }
      }
    }
  }

  // Консольный вывод графа в виде матрицы смежности.
  void ConsolePrintAdjacencyMatrix() {
    cout << "\t";
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      cout << i + 1 << "\t";
    }
    cout << endl;
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      cout << i + 1 << "\t";
      for (size_t j = 0; j < count_of_vertexes_; ++j) {
        cout << adjacency_matrix_[i][j] << "\t";
      }
      cout << endl;
    }
  }

  // Файловый вывод графа в виде матрицы смежности.
  void FilePrintAdjacencyMatrix() {
    ofstream out(output_path_);
    if (!out.is_open()) {
      cerr << "Ошибка вывода!" << endl;
      return;
    }
    out << "\t";
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      out << i + 1 << "\t";
    }
    out << endl;
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      out << i + 1 << "\t";
      for (size_t j = 0; j < count_of_vertexes_; ++j) {
        out << adjacency_matrix_[i][j] << "\t";
      }
      out << endl;
    }
  }

  // Консольный вывод графа в виде списка смежности.
  void ConsolePrintAdjacencyList() {
    vector<vector<int>> adjacency_list = GetAdjacencyList(count_of_vertexes_, adjacency_matrix_);
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      cout << i + 1 << " | ";
      for (int v : adjacency_list[i]) {
        cout << v + 1 << " ";
      }
      cout << endl;
    }
  }

  // Файловый вывод графа в виде списка смежности.
  void FilePrintAdjacencyList() {
    ofstream out(output_path_);
    if (!out.is_open()) {
      cerr << "Ошибка вывода!" << endl;
      return;
    }
    vector<vector<int>> adjacency_list = GetAdjacencyList(count_of_vertexes_, adjacency_matrix_);
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      out << i + 1 << " | ";
      for (int v : adjacency_list[i]) {
        out << v + 1 << " ";
      }
      out << endl;
    }
  }

  // Консольный вывод графа в виде списка ребер.
  void ConsolePrintListOfEdges() {
    vector<pair<int, int>> list_of_edges = GetListOfEdges(count_of_vertexes_, adjacency_matrix_, is_orient_);
    for (auto edge : list_of_edges) {
      cout << edge.first << " " << edge.second << endl;
    }
  }

  // Файловый вывод графа в виде списка ребер.
  void FilePrintListOfEdges() {
    ofstream out(output_path_);
    if (!out.is_open()) {
      cerr << "Ошибка вывода!" << endl;
      return;
    }
    vector<pair<int, int>> list_of_edges = GetListOfEdges(count_of_vertexes_, adjacency_matrix_, is_orient_);
    for (auto edge : list_of_edges) {
      out << edge.first << " " << edge.second << endl;
    }
  }

  // Консольный вывод графа в виде матрицы инцидентности.
  void ConsolePrintIncidenceMatrix() {
    vector<vector<int>>
        incidence_matrix = GetIncidenceMatrix(count_of_vertexes_, count_of_edges_, adjacency_matrix_, is_orient_);
    cout << "\t";
    for (size_t i = 0; i < count_of_edges_; ++i) {
      cout << i + 1 << "\t";
    }
    cout << endl;
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      cout << i + 1 << "\t";
      for (size_t j = 0; j < count_of_edges_; ++j) {
        cout << incidence_matrix[i][j] << "\t";
      }
      cout << endl;
    }
  }

  // Файловый вывод графа в виде матрицы инцидентности.
  void FilePrintIncidenceMatrix() {
    ofstream out(output_path_);
    if (!out.is_open()) {
      cerr << "Ошибка вывода!" << endl;
      return;
    }
    vector<vector<int>>
        incidence_matrix = GetIncidenceMatrix(count_of_vertexes_, count_of_edges_, adjacency_matrix_, is_orient_);
    out << "\t";
    for (size_t i = 0; i < count_of_edges_; ++i) {
      out << i + 1 << "\t";
    }
    out << endl;
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      out << i + 1 << "\t";
      for (size_t j = 0; j < count_of_edges_; ++j) {
        out << incidence_matrix[i][j] << "\t";
      }
      out << endl;
    }
  }

 public:
  // Пустой конструктор класса. Просто инициализирует поля дефолтными значениями.
  Graph() {
    count_of_vertexes_ = 0;
    count_of_edges_ = 0;
    is_orient_ = false;
    output_path_ = "..";
    output_path_ += filesystem::path::preferred_separator;
    output_path_ += "output.txt";
    input_path_ = "..";
    input_path_ += filesystem::path::preferred_separator;
    input_path_ += "input.txt";
  }

  // Конструтор класса с параметрами. Инициализирует поля, согласно входным параметрам.
  Graph(int count_of_vertexes, int count_of_edges, vector<vector<int>> adjacency_matrix, bool is_orient) {
    count_of_vertexes_ = static_cast<size_t>(count_of_vertexes);
    count_of_edges_ = static_cast<size_t>(count_of_edges);
    adjacency_matrix_ = std::move(adjacency_matrix);
    is_orient_ = is_orient;
    output_path_ = "..";
    output_path_ += filesystem::path::preferred_separator;
    output_path_ += "output.txt";
    input_path_ = "..";
    input_path_ += filesystem::path::preferred_separator;
    input_path_ += "input.txt";
  }

  // Вывод графа в виде матрицы смежности, предоставляя выбор способа вывода.
  void PrintAdjacencyMatrix() {
    cout << "Введите 0, если нужно вывести результат в консоль, и любое другое число," << endl
         << "чтобы вывести в файл." << endl << "> ";
    string output_type;
    cin >> output_type;
    if (output_type[0] == '0') {
      ConsolePrintAdjacencyMatrix();
      return;
    }
    FilePrintAdjacencyMatrix();
  }

  // Вывод графа в виде списка смежности, предоставляя выбор способа вывода.
  void PrintAdjacencyList() {
    cout << "Введите 0, если нужно вывести результат в консоль, и любое другое число," << endl
         << "чтобы вывести в файл." << endl << "> ";
    string output_type;
    cin >> output_type;
    if (output_type[0] == '0') {
      ConsolePrintAdjacencyList();
      return;
    }
    FilePrintAdjacencyList();
  }

  // Вывод графа в виде списка ребер, предоставляя выбор способа вывода.
  void PrintListOfEdges() {
    cout << "Введите 0, если нужно вывести результат в консоль, и любое другое число," << endl
         << "чтобы вывести в файл." << endl << "> ";
    string output_type;
    cin >> output_type;
    if (output_type[0] == '0') {
      ConsolePrintListOfEdges();
      return;
    }
    FilePrintListOfEdges();
  }

  // Вывод графа в виде матрицы инцидентности, предоставляя выбор способа вывода.
  void PrintIncidenceMatrix() {
    cout << "Введите 0, если нужно вывести результат в консоль, и любое другое число," << endl
         << "чтобы вывести в файл." << endl << "> ";
    string output_type;
    cin >> output_type;
    if (output_type[0] == '0') {
      ConsolePrintIncidenceMatrix();
      return;
    }
    FilePrintIncidenceMatrix();
  }

  // Реализовывает обход графа, предоставляя выбор способа обхода.
  void GraphTravelsale(int type_of_travelsale) {
    ofstream fout;
    bool is_console = true;
    cout << "Введите 0, если нужно вывести результат в консоль, и любое другое число," << endl
         << "чтобы вывести в файл." << endl << "> ";
    string output_type;
    cin >> output_type;
    if (output_type[0] != '0') {
      fout = ofstream(output_path_);
      is_console = false;
    }
    vector<bool> is_used_vertex(count_of_vertexes_, false);
    int current_component = 0;
    for (size_t i = 0; i < count_of_vertexes_; ++i)
      if (!is_used_vertex[i]) {
        if (is_console)
          cout << current_component + 1 << "-ая компонента:" << endl;
        else
          fout << current_component + 1 << "-ая компонента:" << endl;
        vector<int> vertexes;
        switch (type_of_travelsale) {
          case 0:RecursionDfs(is_used_vertex, i, vertexes);
            break;
          case 1:StackDfs(is_used_vertex, i, vertexes);
            break;
          case 2:Bfs(i, is_used_vertex, vertexes);
            break;
          default:break;
        }
        current_component++;
        for (int vertex : vertexes) {
          if (is_console)
            cout << "Вершина #" << vertex + 1 << endl;
          else
            fout << "Вершина #" << vertex + 1 << endl;
        }
        if (is_console)
          cout << "-------------------------" << endl;
        else
          fout << "-------------------------" << endl;
      }
  }

  // Находит и выводит степень каждой вершины графа.
  void PrintVertexDegree() {
    ofstream fout;
    bool is_console = true;
    cout << "Введите 0, если нужно вывести результат в консоль, и любое другое число," << endl
         << "чтобы вывести в файл." << endl << "> ";
    string output_type;
    cin >> output_type;
    if (output_type[0] != '0') {
      fout = ofstream(output_path_);
      is_console = false;
    }
    for (size_t i = 0; i < count_of_vertexes_; ++i) {
      if (is_console)
        cout << i + 1 << " : ";
      else
        fout << i + 1 << " : ";
      int cnt_out = 0;
      int cnt_in = 0;
      for (size_t j = 0; j < count_of_vertexes_; ++j) {
        if (adjacency_matrix_[i][j])
          cnt_out++;
        if (adjacency_matrix_[j][i])
          cnt_in++;
      }
      if (is_orient_)
        if (is_console)
          cout << "Исходящих = " << cnt_out << ", Входящих = " << cnt_in << endl;
        else
          fout << "Исходящих = " << cnt_out << ", Входящих = " << cnt_in << endl;
      else if (is_console)
        cout << "Степень = " << (cnt_out + cnt_in) / 2 << endl;
      else
        fout << "Степень = " << (cnt_out + cnt_in) / 2 << endl;
    }
  }

  // Выводит количество ребер графа.
  void PrintCountOfEdges() {
    ofstream fout;
    bool is_console = true;
    cout << "Введите 0, если нужно вывести результат в консоль, и любое другое число," << endl
         << "чтобы вывести в файл." << endl << "> ";
    string output_type;
    cin >> output_type;
    if (output_type[0] != '0') {
      fout = ofstream(output_path_);
      is_console = false;
    }
    if (is_orient_)
      if (is_console)
        cout << "Количество дуг = " << count_of_edges_ << endl;
      else
        fout << "Количество дуг = " << count_of_edges_ << endl;
    else if (is_console)
      cout << "Количество ребер = " << count_of_edges_ << endl;
    else
      fout << "Количество ребер = " << count_of_edges_ << endl;
  }
};

// Осуществляет чтение матрицы смежности из потока.
void ReadAdjacencyMatrix(istream &in, Graph &graph) {
  int count_of_vertexes = -1;
  int count_of_edges = -1;
  bool is_orient = false;
  in >> is_orient >> count_of_vertexes >> count_of_edges;
  vector<vector<int>> adjacency_matrix(count_of_vertexes, vector<int>(count_of_vertexes, 0));
  for (size_t i = 0; i < adjacency_matrix.size(); ++i) {
    for (size_t j = 0; j < adjacency_matrix.size(); ++j) {
      in >> adjacency_matrix[i][j];
    }
  }
  graph = Graph(count_of_vertexes, count_of_edges, adjacency_matrix, is_orient);
}

// Осуществляет чтение списка из потока.
void ReadListOfEdges(istream &in, Graph &graph) {
  int count_of_vertexes = -1;
  int count_of_edges = -1;
  bool is_orient = false;
  in >> is_orient >> count_of_vertexes >> count_of_edges;
  vector<pair<int, int>> list_of_edges(count_of_edges);
  for (int i = 0; i < count_of_edges; ++i) {
    in >> list_of_edges[i].first >> list_of_edges[i].second;
  }
  graph = Graph(count_of_vertexes,
                count_of_edges,
                FromEdgesListToAdjacencyMatrix(count_of_vertexes, list_of_edges, is_orient),
                is_orient);
}

// Осуществляет чтение списка смежности из потока.
void ReadAdjacencyList(istream &in, Graph &graph) {
  int count_of_vertexes = -1;
  int count_of_edges = -1;
  bool is_orient = false;
  in >> is_orient >> count_of_vertexes >> count_of_edges;
  vector<vector<int>> adjacency_list(count_of_vertexes);
  string line;
  getline(in, line);
  for (int i = 0; i < count_of_vertexes; ++i) {
    getline(in, line);
    if (line.empty())
      continue;
    istringstream stream(line);
    int to;
    while (stream >> to) {
      adjacency_list[i].push_back(to - 1);
      if (!is_orient)
        adjacency_list[to - 1].push_back(i);
    }
  }
  graph = Graph(count_of_vertexes, count_of_edges,
                FromAdjacencyListToAdjacencyMatrix(count_of_vertexes, adjacency_list),
                is_orient);
}

// Осуществляет чтение матрицы инцидентности из потока.
void ReadIncidenceMatrix(istream &in, Graph &graph) {
  int count_of_vertexes = -1;
  int count_of_edges = -1;
  bool is_orient = false;
  in >> is_orient >> count_of_vertexes >> count_of_edges;
  vector<vector<int>> incidence_matrix(count_of_vertexes, vector<int>(count_of_edges));
  for (int i = 0; i < count_of_vertexes; ++i) {
    for (int j = 0; j < count_of_edges; ++j) {
      in >> incidence_matrix[i][j];
    }
  }
  auto adjancy_matrix =
      FromIncidenceMatrixToAdjacencyMatrix(count_of_vertexes, count_of_edges, incidence_matrix, is_orient);
  graph = Graph(count_of_vertexes, count_of_edges,
                adjancy_matrix,
                is_orient);
}

// Осуществляет чтение графа. Возвращает true, если граф был считан успешно.
bool ReadGraph(Graph &graph, string &input_path) {
  cout << "Выберите способ ввода графа, введите 0, если ввод будет осуществлен с консоли," << endl
       << "или любое другое число, если необходимо считать граф с файла: " << endl << ">";
  string is_file_read;
  cin >> is_file_read;
  ifstream fin;
  int graph_type;
  if (is_file_read[0] != '0') {
    fin = ifstream(input_path);
    if (!fin.is_open()) {
      cerr << "Ошибка чтения файла!";
      return false;
    }
    fin >> graph_type;
  } else {
    cin >> graph_type;
  }
  switch (graph_type) {
    case 0:ReadAdjacencyMatrix(is_file_read[0] == '0' ? cin : fin, graph);
      break;
    case 1:ReadAdjacencyList(is_file_read[0] == '0' ? cin : fin, graph);
      break;
    case 2:ReadListOfEdges(is_file_read[0] == '0' ? cin : fin, graph);
      break;
    case 3:ReadIncidenceMatrix(is_file_read[0] == '0' ? cin : fin, graph);
      break;
    default:cerr << "Неверная команда!" << endl;
      return false;
  }
  return true;
}

// Выводит меню.
void PrintMenu() {
  cout << "Меню:" << endl;
  cout << "help: Вывести меню" << endl;
  cout << "1: Вывести матрицу смежности" << endl;
  cout << "2: Вывести список смежности" << endl;
  cout << "3: Вывести список ребер" << endl;
  cout << "4: Вывести матрицу инцидентности" << endl;
  cout << "5: Обойти граф с помощью dfs (рекурсивный алгоритм)" << endl;
  cout << "6: Обойти граф с помощью dfs (нерекурсивный алгоритм)" << endl;
  cout << "7: Обойти граф с помощью bfs" << endl;
  cout << "8: Подсчет количества степеней" << endl;
  cout << "9: Подсчет количества ребер" << endl;
  cout << "0: Закончить работу с этим графом" << endl;
}

// Обрабатывает запросы пользователя.
void Program(Graph &graph) {
  PrintMenu();
  string command_type;
  while (true) {
    cout << "> ";
    cin >> command_type;
    switch (command_type[0]) {
      case '0':return;
      case '1':graph.PrintAdjacencyMatrix();
        break;
      case '2':graph.PrintAdjacencyList();
        break;
      case '3':graph.PrintListOfEdges();
        break;
      case '4':graph.PrintIncidenceMatrix();
        break;
      case '5':graph.GraphTravelsale(0);
        break;
      case '6':graph.GraphTravelsale(1);
        break;
      case '7':graph.GraphTravelsale(2);
        break;
      case '8':graph.PrintVertexDegree();
        break;
      case '9':graph.PrintCountOfEdges();
        break;
      default:
        if (command_type == "help") {
          PrintMenu();
          break;
        }
        cerr << "Неверная команда, повторите попытку!" << endl;
        break;
    }
  }
}

// Запускает основную программу и осуществляет повтор решения.
int main() {
  system("chcp 65001");
  string output_path;
  string input_path;
  output_path = "..";
  output_path += filesystem::path::preferred_separator;
  output_path += "output.txt";
  input_path = "..";
  input_path += filesystem::path::preferred_separator;
  input_path += "input.txt";
  while (true) {
    Graph graph;
    if (ReadGraph(graph, input_path)) {
      Program(graph);
    }
    cout << "Для выхода из программы введите 0, иначе любое другое число" << endl << "> ";
    string key;
    cin >> key;
    if (key[0] == '0')
      break;
  }
}