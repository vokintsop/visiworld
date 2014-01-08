// graph.h
// ������� ���������� ��� ������ � �������

#include <vector>

// ������� ����� - ������� ������
struct graphElem
{
   int vert_num;// ����� ������� 
   int next;// id  �������� �������
   double weight;// ���
   graphElem(int vert_num1 = -1, double weight = -1, int next1 = -1)
   {
      vert_num = vert_num1;
      next = next1;
   }
};

// �������� ��������� ���������� ����
// �������� ��� ������ ������ �����
struct graph
{
  std::vector<graphElem> elems; // ������ ���� ��������� ���� �������
  std::vector<int> vertex; // ������ ���������� �� ������ ������ ������� �� ������ ������
  
  void addVertex(int v) // ��������� ������� � ������� v
  {
    if ((int)vertex.size() <= v)
    {
      int predSize = vertex.size();
      vertex.resize(v + 1);
      for (int i = predSize; i < (int)vertex.size(); ++i)
        vertex[i] = -1;
    }  
  }
  void addEdge(int from, int to, double weight = 0) // ��������� ����� (from; to) -- !! ���������������!!
  {
     addVertex(std::max(from, to));
     if (vertex[from] == -1)// ���� ������ ������
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