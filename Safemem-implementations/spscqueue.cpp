#include <functional>
#include <iostream>
#include <chrono>
#include <safemem.h>
#include <atomic>
#include <thread>
using namespace std;


struct Node {
    int value;
    atomic<Node*> next;
    Node(int val) : value(val), next(nullptr) {}
};

//Removed redundant atomic in tail since tail was not touched by consumer
class safememSPSCqueue {
    private:    
        Node* head;         //consumer reads data here
        Node* tail;         //producer enters new data here (Changed to plain pointer)

    public:
        safememSPSCqueue() {
            void* raw_mem=safemem(sizeof(Node));  //used safemem for fast memory allocation
            Node* dummy=new (raw_mem) Node(-1);     //create object at allocated new memory  
                                                        //dummy node so that tail/head are not empty 
            
            head=dummy;
            tail=dummy;   // plain assignment
        }
        
        //Producer thread works here 
        void push(int val) {
            void* raw_mem=safemem(sizeof(Node));
            Node* newNode=new(raw_mem) Node(val);
            
            Node* oldTail=tail; // plain read

            newNode->value=val;
            oldTail->next.store(newNode,std::memory_order_release);

            tail=newNode; // plain assignment
        }
        
        //consumer thread works here
        bool pop(int& result) {
            Node* currentHead=head;
            Node* nextNode=currentHead->next.load(std::memory_order_acquire);

            if(nextNode==nullptr) {
                return false;
            }

            result=nextNode->value;
            head=nextNode;

            safe_free(currentHead);
            return true;
        }
};

const int test=1000000;

void producer(safememSPSCqueue& q) {
    for(int i=1;i<=test;i++) {
        q.push(i);
    }
}


void consumer(safememSPSCqueue& q, bool &verified, double &latency) {
    int val;
    int count=0;
    int expect=1;
    bool correct=true;
    
    auto start=chrono::steady_clock::now();

    while(count<test) {
        if(q.pop(val)) {
            if(val!=expect) correct=false;
            expect++;
            count++; 
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration=chrono::duration_cast<chrono::nanoseconds>(end-start).count();
    latency=(double)duration/test;
    verified=correct;
}

int main() {
    safememSPSCqueue qu;
    bool verified=false;
    double avglatency=0;
    
    cout << "Starting SPSC Benchmark with 1 Million Operations..." << endl;

    thread t1(producer,ref(qu));
    thread t2(consumer,ref(qu), ref(verified),ref(avglatency));

    t1.join();
    t2.join();
   
    cout << "------------------------------------------" << endl;
    cout << "Verification: " << (verified ? "PASSED (No Data Loss)" : "FAILED") << endl;
    cout << "Avg Latency (Push+Pop+Logic): " << avglatency << " ns" << endl;
    cout << "Throughput: " << (1000.0 / avglatency) << " Million ops/sec" << endl;
    cout << "------------------------------------------" << endl;


    return 0;
}
