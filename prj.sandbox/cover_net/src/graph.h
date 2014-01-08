// graph.h
// простая структурка для работы с графами

#include <vector>

// вершина графа - элемент списка
struct graphElem
{
   int vert_num;// номер вершины 
   int next;// id  слеующей вершины
   double weight;// вес
   graphElem(int vert_num1 = -1, double weight = -1, int next1 = -1)
   {
      vert_num = vert_num1;
      next = next1;
   }
};

// структра реализует простейший граф
// хранится как вектор списка ребер
struct graph
{
  std::vector<graphElem> elems; // массив всех элементов всех списков
  std::vector<int> vertex; // массив указателей по номеру каждой вершины на начало списка
  
  void addVertex(int v) // добавляет вершину с номером v
  {
    if ((int)vertex.size() <= v)
    {
      int predSize = vertex.size();
      vertex.resize(v + 1);
      for (int i = predSize; i < (int)vertex.size(); ++i)
        vertex[i] = -1;
    }  
  }
  void addEdge(int from, int to, double weight = 0) // добавляет ребро (from; to) -- !! ориентированное!!
  {
     addVertex(std::max(from, to));
     if (vertex[from] == -1)// если список пустой
     {
        vertex[from] = elems.size();
        elems.push_back(graphElem(to, weight));
     }
     else 
     {
        elems.push_back(graphElem(to, weight, vertex[from]));
        vertex[from] = elems.size() - 1;
     }
  }
};