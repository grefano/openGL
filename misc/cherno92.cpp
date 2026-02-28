#include <iostream>
void log(const char* str){
    std::cout << str << "\0";
}

template<typename T>
class Vector{
    T* data = nullptr; 
    size_t size;
    public:
    Vector(){
        data = new T[1];
        data[0] = 2;
        size = 1;
    }
    ~Vector(){
        log("deleted vector");
        delete[] data;
    }
    void push(const T& value){
        realloc(size+1);
        data[size-1] = value;
    }
    void push(T&& value){
        realloc(size+1);
        data[size-1] = std::move(value);
    }
    template<typename... Args>
    T& emplace_back(Args&&... args){
        realloc(size+1);
        new(&data[size-1]) T(std::forward<Args>(args)...);
        return data[size-1];

    }
    void realloc(size_t newsize){
        T* newdata = new T[newsize];
        // memcpy(newdata, data, size*sizeof(T));
        for(size_t i = 0; i < size; i++)
        newdata[i] = std::move(data[i]);
        delete[] data;
        data = newdata;
        size = newsize;
        newdata = nullptr;
        
    }
    const T& operator[](size_t index) const{
        return data[index];
    }
    size_t get_size() const{
        return size;
    }
    
};
template<typename T>
void printVector(const Vector<T>& vector){
    std::cout << "vector: ";
    for(size_t i = 0; i < vector.get_size(); i++){
        std::cout << vector[i];
    }
    std::cout << "\0";
}

int main(){
    Vector<int> myvector;


    myvector.push(5);
    myvector.push(8);
    myvector.emplace_back(9);
    myvector.emplace_back(9);
    printVector(myvector);
    
    

    // log((const char*) myvector[0]);
    // log((const char*) myvector[1]);

    return 0;
}