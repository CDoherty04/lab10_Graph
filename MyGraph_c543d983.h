#ifndef _MYGRAPH_H_
#define _MYGRAPH_H_

#include <iostream>
#include <fstream>

#include "MyVector_c543d983.h"
#include "MyLinkedList_c543d983.h"
#include "MyQueue_c543d983.h"
#include "MyStack_c543d983.h"
#include "MyHashTable_c543d983.h"


// the graph is supposed to be a undirected graph
// no multi-edge is allowed (assumed to be ensured from the input)

typedef long long VertexIDType;
typedef long long EdgeIDType;

template <typename VertexDataType, typename EdgeDataType>
class MyGraph
{
  public:

    class Vertex
    {
      public: 
        // default constructor
        Vertex()
        {
            return;
        }

        // vertex copy constructor
        explicit Vertex(const VertexDataType& vertex_data) :
            data(vertex_data)
        {
            return;
        }

        // vertex move constructor
        explicit Vertex(VertexDataType && vertex_data) :
            data(std::move(vertex_data))
        {
            return;
        }

        // copy assignment
        Vertex& operator=(const Vertex& rhs)
        {
            id = rhs.id;
            tag = rhs.tag;
            data = rhs.data;
            return *this;
        }

        // move assignment
        Vertex& operator=(Vertex && rhs)
        {
            id = rhs.id;
            tag = rhs.tag;
            data = std::move(rhs.data);
            return *this;
        }

        VertexIDType id;        // the unique ID of the vertex
        int tag;                // a reserved tag variable
        VertexDataType data;    // the property associated with the vertex

        friend class MyGraph;

    };

    class Edge
    {
      public:
        // default constructor
        Edge()
        {
            return;
        }

        // copy constructor
        // sid: source vertex ID; vid: target vertex ID
        explicit Edge(const VertexIDType sid, const VertexIDType tid, const EdgeDataType& edge_data) :
            src(sid),
            tgt(tid),
            data(edge_data)
        {
            return;
        }

        // move constructor
        // sid: source vertex ID; vid: target vertex ID
        explicit Edge(const VertexIDType sid, const VertexIDType tid, EdgeDataType && edge_data) :
            src(sid),
            tgt(tid),
            data(std::move(edge_data))
        {
            return;
        }

        // copy assignment
        Edge& operator=(const Edge& rhs)
        {
            id = rhs.id;
            src = rhs.src;
            tgt = rhs.tgt;
            tag = rhs.tag;
            data = rhs.data;
            return *this;
        }

        // move assignment
        Edge& operator=(Edge && rhs)
        {
            id = rhs.id;
            src = rhs.src;
            tgt = rhs.tgt;
            tag = rhs.tag;
            data = std::move(rhs.data);
            return *this;
        }
      
        EdgeIDType id;          // the unique ID of the edge
        VertexIDType src, tgt;  // the IDs for the source vertex and the target vertex of the edge
        int tag;                // the reserved tag varaible
        EdgeDataType data;      // the property associated with the edge

        friend class MyGraph;

    };

    // default constructor of graph
    // initialize the graph with a size of 0
    MyGraph() :
        current_vertex_ID(0),
        current_edge_ID(0),
        num_vertices(0),
        num_edges(0),
        vertex_set{0},
        edge_set{0},
        vertex_map{},
        edge_map{},
        adj_list{}
    {   
        return;
    }

    // destructor
    ~MyGraph()
    {
        for(size_t i = 0; i < vertex_set.size(); ++ i)
        {
            delete vertex_set[i];
        }
        for(size_t i = 0; i < edge_set.size(); ++ i)
        {
            delete edge_set[i];
        }
        for(size_t i = 0; i < adj_list.size(); ++ i)
        {
            adj_list[i]->clear();
            delete adj_list[i];
        }
        return;
    }

    // adds an vertex v to the graph (copy)
    // assign ID according the to oder of vertex insertion
    // for example, assign ID 1 to the first vertex, 2 to the second vertex, ..., and n to the nth vertex
    // returns the assigned vertex ID for v
    VertexIDType addVertex(const VertexDataType& v_data)
    {
        // code begins
        Vertex* v = new Vertex(v_data);
        v->id = current_vertex_ID;
        v->tag = 0;
        vertex_set.push_back(v);
        vertex_map.insert(HashedObj<VertexIDType, size_t>(current_vertex_ID, num_vertices));
        adj_list.push_back(new MyLinkedList<EdgeIDType>());
        ++num_vertices;
        return current_vertex_ID++;
        // code ends
    }

    // adds an vertex v to the graph (move)
    // assign ID according the to oder of vertex insertion
    // for example, assign ID 1 to the first vertex, 2 to the second vertex, ..., and n to the nth vertex
    // returns the assigned vertex ID for v
    VertexIDType addVertex(VertexDataType && v_data)
    {
        // code begins
        Vertex* v = new Vertex(std::move(v_data));
        v->id = current_vertex_ID;
        v->tag = 0;
        vertex_set.push_back(v);
        vertex_map.insert(HashedObj<VertexIDType, size_t>(current_vertex_ID, num_vertices));
        adj_list.push_back(new MyLinkedList<EdgeIDType>());
        ++num_vertices;
        return current_vertex_ID++;
        // code ends
    }

    // retrieves the information of the vertex specified by the vid
    // returns the vertex
    Vertex* getVertex(const VertexIDType vid)
    {
        // code begins
        HashedObj<VertexIDType, size_t> data;
        if (vertex_map.retrieve(vid, data)) {
            return vertex_set[data.value];
        }
        return nullptr;
        // code ends
    }

    // adds an edge e between the source node src and the target node tgt (copy)
    // adds to both of the src's adjacency list and tgt's adjacency list (as undirected graph)
    // assign ID according the to oder of edge insertion
    // for example, assign ID 1 to the first edge, 2 to the second edge, ..., and n to the nth edge
    // returns the assigned edge ID for e
    EdgeIDType addEdge(const VertexIDType sid, const VertexIDType tid, const EdgeDataType& e_data)
    {
        // code begins
        // No multi-edge allowed, so check first
        EdgeIDType dummy;
        if (probeEdge(sid, tid, dummy)) return -1;
        Edge* e = new Edge(sid, tid, e_data);
        e->id = current_edge_ID;
        e->tag = 0;
        edge_set.push_back(e);
        edge_map.insert(HashedObj<EdgeIDType, size_t>(current_edge_ID, num_edges));
        // Add to both adjacency lists
        size_t spos = vertexID2SetPos(sid);
        size_t tpos = vertexID2SetPos(tid);
        adj_list[spos]->push_back(current_edge_ID);
        adj_list[tpos]->push_back(current_edge_ID);
        ++num_edges;
        return current_edge_ID++;
        // code ends
    }

    // adds an edge e between the source node src and the target node tgt (copy)
    // adds to both of the src's adjacency list and tgt's adjacency list (as undirected graph)
    // assign ID according the to oder of vertex insertion
    // for example, assign ID 1 to the first vertex, 2 to the second vertex, ..., and n to the nth vertex
    // returns the assigned edge ID for e
    EdgeIDType addEdge(const VertexIDType sid, const VertexIDType tid, EdgeDataType && e_data)
    {
        // code begins
        EdgeIDType dummy;
        if (probeEdge(sid, tid, dummy)) return -1;
        Edge* e = new Edge(sid, tid, std::move(e_data));
        e->id = current_edge_ID;
        e->tag = 0;
        edge_set.push_back(e);
        edge_map.insert(HashedObj<EdgeIDType, size_t>(current_edge_ID, num_edges));
        size_t spos = vertexID2SetPos(sid);
        size_t tpos = vertexID2SetPos(tid);
        adj_list[spos]->push_back(current_edge_ID);
        adj_list[tpos]->push_back(current_edge_ID);
        ++num_edges;
        return current_edge_ID++;
        // code ends
    }

    // retrieves the information of the edge specified by the eid
    // returns the edge
    Edge* getEdge(const EdgeIDType eid)
    {
        // code begins
        HashedObj<EdgeIDType, size_t> data;
        if (edge_map.retrieve(eid, data)) {
            return edge_set[data.value];
        }
        return nullptr;
        // code ends
    }

    // checks whether an exists between src and tgt
    // if yes, return TRUE and record the corresponding edge ID in eid
    // if no, return FALSE
    bool probeEdge(const VertexIDType sid, const VertexIDType tid, EdgeIDType& eid)
    {
        // code begins
        size_t spos = vertexID2SetPos(sid);
        for (auto it = adj_list[spos]->begin(); it != adj_list[spos]->end(); ++it) {
            Edge* e = getEdge(*it);
            if ((e->src == sid && e->tgt == tid) || (e->src == tid && e->tgt == sid)) {
                eid = e->id;
                return true;
            }
        }
        return false;
        // code ends
    }

    // returns the degree of the vertex specified by vid
    size_t degree(const VertexIDType vid)
    {
        // code begins
        size_t vpos = vertexID2SetPos(vid);
        return adj_list[vpos]->size();
        // code ends
    }

    // deletes the vertex specified by vid from the graph
    // also delete all edges associated with it
    // expected time complexity: O(d^2), where d is the average degree of the graph
    void deleteVertex(const VertexIDType vid)
    {
        // code begins
        size_t vpos = vertexID2SetPos(vid);
        // 1. Collect all edges to delete
        MyLinkedList<EdgeIDType>* edges = adj_list[vpos];
        MyVector<EdgeIDType> to_delete;
        for (auto it = edges->begin(); it != edges->end(); ++it) {
            to_delete.push_back(*it);
        }
        // 2. Delete all incident edges
        for (size_t i = 0; i < to_delete.size(); ++i) {
            deleteEdge(to_delete[i]);
        }
        // 3. Delete the vertex
        delete vertex_set[vpos];
        delete adj_list[vpos];
        // 4. Shift all elements after vpos down by 1
        for (size_t i = vpos + 1; i < vertex_set.size(); ++i) {
            vertex_set[i - 1] = vertex_set[i];
            adj_list[i - 1] = adj_list[i];
            vertex_map.remove(vertex_set[i]->id);
            vertex_map.insert(HashedObj<VertexIDType, size_t>(vertex_set[i]->id, i - 1));
        }
        vertex_set.pop_back();
        adj_list.pop_back();
        vertex_map.remove(vid);
        --num_vertices;
        // 5. For all edges, if src or tgt > vpos, decrement their index by 1
        for (size_t i = 0; i < edge_set.size(); ++i) {
            if (vertexID2SetPos(edge_set[i]->src) > vpos) {
                edge_set[i]->src = vertex_set[vertexID2SetPos(edge_set[i]->src) - 1]->id;
            }
            if (vertexID2SetPos(edge_set[i]->tgt) > vpos) {
                edge_set[i]->tgt = vertex_set[vertexID2SetPos(edge_set[i]->tgt) - 1]->id;
            }
        }
        // 6. For all adjacency lists, remove any references to deleted edges (already done in deleteEdge)
        // code ends
    }

    // deletes the edge specified by eid from the graph
    // expected time complexity: O(d), where d is the average degree of the graph
    void deleteEdge(const EdgeIDType eid)
    {
        // code begins
        size_t epos = edgeID2SetPos(eid);
        Edge* e = edge_set[epos];
        size_t spos = vertexID2SetPos(e->src);
        size_t tpos = vertexID2SetPos(e->tgt);
        // Remove from adjacency lists
        for (auto it = adj_list[spos]->begin(); it != adj_list[spos]->end(); ++it) {
            if (*it == eid) { adj_list[spos]->erase(it); break; }
        }
        for (auto it = adj_list[tpos]->begin(); it != adj_list[tpos]->end(); ++it) {
            if (*it == eid) { adj_list[tpos]->erase(it); break; }
        }
        // Remove from edge_set and edge_map
        delete edge_set[epos];
        edge_set[epos] = edge_set[num_edges - 1];
        edge_set.pop_back();
        EdgeIDType last_id = edge_set[epos]->id;
        edge_map.remove(eid);
        if (epos != num_edges - 1) {
            edge_map.remove(last_id);
            edge_map.insert(HashedObj<EdgeIDType, size_t>(last_id, epos));
        }
        --num_edges;
        // code ends
    }

    // performs breath-first-search on the graph, starting from v_src
    // record the path in path
    // outgoing edge visit order should be determined based on their ID (smaller ID visited earlier)
    // the source vertex should also be included in the traversal path
    void breadthFirstSearch(const VertexIDType v_src, MyVector<VertexIDType>& path)
    {
        // code begins
        path.resize(0);
        MyVector<bool> visited(vertex_set.size(), false);
        MyQueue<VertexIDType> q;
        size_t src_pos = vertexID2SetPos(v_src);
        q.enqueue(v_src);
        visited[src_pos] = true;
        while (!q.empty()) {
            VertexIDType vid = q.front();
            q.dequeue();
            path.push_back(vid);
            size_t vpos = vertexID2SetPos(vid);
            // Collect and sort outgoing edge IDs
            MyVector<EdgeIDType> edges;
            for (auto it = adj_list[vpos]->begin(); it != adj_list[vpos]->end(); ++it) {
                edges.push_back(*it);
            }
            std::sort(edges.begin(), edges.end());
            for (size_t i = 0; i < edges.size(); ++i) {
                Edge* e = getEdge(edges[i]);
                VertexIDType neighbor = (e->src == vid) ? e->tgt : e->src;
                size_t npos = vertexID2SetPos(neighbor);
                if (!visited[npos]) {
                    q.enqueue(neighbor);
                    visited[npos] = true;
                }
            }
        }
        // code ends
    }

    // performs depth-first-search on the graph, starting from v_src
    // record the path in path
    // outgoing edge visit order should be determined based on their ID (smaller ID visited earlier)
    // the source vertex should also be included in the traversal path
    void depthFirstSearch(const VertexIDType v_src, MyVector<VertexIDType>& path)
    {
        // code begins
        path.resize(0);
        MyVector<bool> visited(vertex_set.size(), false);
        MyStack<VertexIDType> s;
        size_t src_pos = vertexID2SetPos(v_src);
        s.push(v_src);
        while (!s.empty()) {
            VertexIDType vid = s.top();
            s.pop();
            size_t vpos = vertexID2SetPos(vid);
            if (visited[vpos]) continue;
            visited[vpos] = true;
            path.push_back(vid);
            // Collect and sort outgoing edge IDs in reverse order for stack
            MyVector<EdgeIDType> edges;
            for (auto it = adj_list[vpos]->begin(); it != adj_list[vpos]->end(); ++it) {
                edges.push_back(*it);
            }
            std::sort(edges.begin(), edges.end());
            for (int i = static_cast<int>(edges.size()) - 1; i >= 0; --i) {
                Edge* e = getEdge(edges[i]);
                VertexIDType neighbor = (e->src == vid) ? e->tgt : e->src;
                size_t npos = vertexID2SetPos(neighbor);
                if (!visited[npos]) {
                    s.push(neighbor);
                }
            }
        }
        // code ends
    }

    // determines whether the two nodes vid1 and vid2 are connected
    // if yes, return TRUE
    // if no, return FASLE
    bool isConnected(const VertexIDType vid1, const VertexIDType vid2)
    {
        // code begins
        if (vid1 == vid2) return true;
        MyVector<bool> visited(vertex_set.size(), false);
        MyQueue<VertexIDType> q;
        size_t src_pos = vertexID2SetPos(vid1);
        q.enqueue(vid1);
        visited[src_pos] = true;
        while (!q.empty()) {
            VertexIDType vid = q.front();
            q.dequeue();
            if (vid == vid2) return true;
            size_t vpos = vertexID2SetPos(vid);
            for (auto it = adj_list[vpos]->begin(); it != adj_list[vpos]->end(); ++it) {
                Edge* e = getEdge(*it);
                VertexIDType neighbor = (e->src == vid) ? e->tgt : e->src;
                size_t npos = vertexID2SetPos(neighbor);
                if (!visited[npos]) {
                    q.enqueue(neighbor);
                    visited[npos] = true;
                }
            }
        }
        return false;
        // code ends
    }

    // returns the number of vertices in the graph
    size_t numVertices(void) const
    {
        return num_vertices;
    }

    // returns the number of edges in the graph
    size_t numEdges(void) const
    {
        return num_edges;
    }

    // checks whether the graph is empty (no vertex)
    bool empty(void) const
    {
        return (num_vertices == 0);
    }

    // prints the content in vertex_set
    void printInfoVertexSet(void) 
    {
        for(size_t i = 0; i < num_vertices; ++ i)
        {
            std::cout << "Array Position: " << i << "\t";
            std::cout << "Vertex ID: " << vertex_set[i]->id << "\t";
            std::cout << "Vertex data: " << vertex_set[i]->data << std::endl;
        }
        return;
    }

    // prints the content in edge_set
    void printInfoEdgeSet(void) 
    {
        for(size_t i = 0; i < num_edges; ++ i)
        {
            std::cout << "Array Position: " << i << "\t";
            std::cout << "Edge ID: " << edge_set[i]->id << "\t";
            std::cout << "Edge ends: " << edge_set[i]->src << "\t" << edge_set[i]->tgt << "\t";
            std::cout << "Edge data: " << edge_set[i]->data << std::endl;
        }
        return;
    }

    // prints the content in vertex_map
    void printInfoVertexMap(void) 
    {
        MyVector<VertexIDType> v_keys;
        vertex_map.keys(v_keys);
        for(size_t i = 0; i < v_keys.size(); ++ i)
        {
            std::cout << "VertexID-Position:\t" << v_keys[i] << "\t" << vertexID2SetPos(v_keys[i]) << std::endl;
        }
        return;
    }

    // prints the content in edge_map
    void printInfoEdgeMap(void) 
    {
        MyVector<EdgeIDType> e_keys;
        edge_map.keys(e_keys);
        for(size_t i = 0; i < e_keys.size(); ++ i)
        {
            std::cout << "EdgeID-Position:\t" << e_keys[i] << "\t" << edgeID2SetPos(e_keys[i]) << std::endl;
        }
        return;
    }

    // prints the content in the adjacency list
    void printAdjList(void) 
    {
        for(size_t i = 0; i < adj_list.size(); ++ i)
        {
            std::cout << "Vertex: " << vertex_set[i]->id << " degree: " << degree(vertex_set[i]->id) << "\tIt is connected with: ";
            for(auto itr = adj_list[i]->begin(); itr != adj_list[i]->end(); ++ itr)
            {
                if(vertex_set[i]->id == getEdge(*itr)->src)
                    std::cout << getEdge(*itr)->tgt << "\t";
                else if(vertex_set[i]->id == getEdge(*itr)->tgt)
                    std::cout << getEdge(*itr)->src << "\t";
                else
                    throw std::domain_error("MyGraph::printAdjList: edge information may have been corrupted.");
            }
            std::cout << std::endl;
        }
    }

  private:
    VertexIDType current_vertex_ID;                 // the ID to be assigned to the next vertex (ID are assigned based on the order of insertion)
    EdgeIDType current_edge_ID;                     // the ID to be assigned to the next edge (IDs are assigned based on the order of insertion)
    size_t num_vertices;                            // the number of vertices
    size_t num_edges;                               // the number of edges
    MyVector<Vertex*> vertex_set;                   // the set of vertices
    MyVector<Edge*> edge_set;                       // the set of edges
    MyHashTable<VertexIDType, size_t> vertex_map;   // the mapping between a vertex ID and its index in vertex_set
    MyHashTable<EdgeIDType, size_t> edge_map;       // the mapping between an edge ID and its index in vertex_set
    MyVector<MyLinkedList<EdgeIDType>* > adj_list;  // the adjacency list (stores the IDs for the corresponding edges)

    // maps the vertex ID to its position in the vertex_set array
    size_t vertexID2SetPos(const VertexIDType vid)
    {
        // code begins
        HashedObj<VertexIDType, size_t> data;
        if (vertex_map.retrieve(vid, data)) {
            return data.value;
        }
        throw std::domain_error("MyGraph::vertexID2SetPos: vertex ID not found.");
        // code ends
    }

    // maps the edge ID to its position in the edge_set array
    size_t edgeID2SetPos(const VertexIDType eid)
    {
        // code begins
        HashedObj<EdgeIDType, size_t> data;
        if (edge_map.retrieve(eid, data)) {
            return data.value;
        }
        throw std::domain_error("MyGraph::edgeID2SetPos: edge ID not found.");
        // code ends
    }

    // gets the vertex ID for one in a given position in the vertex_set array
    VertexIDType vertexSetPos2ID(const size_t vpos)
    {
        // code begins
        return vertex_set[vpos]->id;
        // code ends
    }

    // gets the edge ID for one in a given position in the edge_set array
    EdgeIDType edgeSetPos2ID(const size_t epos)
    {
        // code begins
        return edge_set[epos]->id;
        // code ends
    }
  
};

#endif  // _MYGRAPH_H_