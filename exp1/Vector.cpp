#include <iostream>
#include <stdexcept>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <cmath>
#include <algorithm>

using namespace std;
typedef int Rank; //秩
# define DEFAULT_CAPACITY 3 //默认的初始容量(实际应用中可设置为更大)

class Complex {
public:
    double real;
    double imag;

    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}

    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    bool operator==(const Complex& other) const {
        return real == other.real && imag == other.imag;
    }

    bool operator<(const Complex& other) const {
        if (modulus() == other.modulus()) {
            return real < other.real;
        }
        return modulus() < other.modulus();
    }
};

template < typename T> 
class Vector { //向量模板类
protected:
	Rank size; int capacity; T* elem; //规模、容量、数据区
	void copyFrom(T const* A, Rank lo, Rank hi){//以数组区间A[lo, hi)为蓝本复制向量
	    size = hi - lo;
        elem=new T[capacity=2*( hi-lo)];size =0;//分配空间，规模清零
        while(lo< hi)//A[lo, hi)内的元素逐一
        elem[size++]=A[lo++];//复制至elem[0, hi-lo)	
	} //复制数组区间A[lo, hi)
	void expand(){
		if(size<capacity)return; //尚未满员时，不必扩容
		if(capacity<DEFAULT_CAPACITY) capacity =DEFAULT_CAPACITY;//不低于最小容量
		T*oldElem=elem;elem=new T[capacity <<=1];//容量加倍
		for(int i=0;i<size;i++)
		   elem[i]=oldElem[i];//复制原向量内容(T为基本类型，或已重载赋值操作符'=') 
		delete []oldElem;//释放原空间
    }
	 //空间不足时扩容
	void shrink(){
		if (capacity<DEFAULT_CAPACITY <<1)return;//不致收缩到DEFAULT_CAPACITY以下
		if (size <<2>capacity)return;//以25%为界
		T*oldElem=elem;elem=new T[capacity >>=1];//容量减半
		for(int i=0;i<size;i++)elem[i]=oldElem[i];//复制原向量内容
		delete []oldElem;//释放原空间
    }
	 //装填因子过小时压缩
	bool bubble(Rank lo, Rank hi){
		bool sorted =true;//整体有序标志
        while(++lo<hi)//自左向右，逐一检查各对相邻元素
           if(elem[lo-1]>elem[lo]){//若逆序，则
             sorted=false; //意味着尚未整体有序，并需要与
             swap(elem[lo-1],elem[lo]);//通过交换使局部有序
           }
        return sorted;//返回有序
    }
	 //扫描交换
	void bubbleSort(Rank lo, Rank hi){//assert:0 <= lo<hi <=size 
	  while(!bubble(lo, hi--));}//逐趟做扫描交换，直至全序
	 //起泡排序算法
	Rank max(Rank lo, Rank hi){
	if (lo >= hi) return lo;
    Rank maxIndex = lo;
    for (Rank i = lo + 1; i < hi; ++i) {
        if (elem[i] > elem[maxIndex]) {
            maxIndex = i;
        }
    }
    return maxIndex;
} //选取最大元素
	void selectionSort(Rank lo, Rank hi){
	for (Rank i = lo; i < hi - 1; ++i) {
        Rank maxIndex = max(i, hi);
        swap(elem[i], elem[maxIndex]);
    }
}//选择排序算法
	void merge(Rank lo, Rank mi, Rank hi){//各自有序的子向量[lo,mi)和[mi, hi) 
       T*A=elem+lo;//合并后的向量A[0, hi-lo)=elem[lo, hi) 
       int lb=mi-lo;T*B= new T[lb];//前子向量B[0,lb)=elem[lo, mi) 
	   for (Rank i=0;i<lb;B[i]=A[i++]);//复制前子向量
       int lc= hi-mi;T*C=elem +mi;//后子向量C[0,lc)=elem[mi,hi) 
       for (Rank i=0,j=0,k=0;(j<lb)||(k<lc);){//B[j]和C[k]中的小者续至A未尾
	     if((j<lb)&&(!(k<lc)||(B[j]<=C[k])))A[i++]=B[j++]; 
         if((k<lc)&&(!(j<lb)||(C[k]< B[j])))A[i++]=C[k++]; 
       }
	   delete [] B;//释放临时空间B 
	}//归并后得到完整的有序向量[lo,hi)
	 //归并算法
	void mergeSort(Rank lo, Rank hi){//0<=lo<hi <=size 
	   if(hi-lo<2)return; //单元素区间自然有序，否则... 
       int mi=(lo+ hi)/2;//以中点为界
       mergeSort (lo, mi);mergeSort(mi,hi );//分别排序
	   merge (lo,mi,hi);//归并 
	}//归并排序算法
	Rank partition(Rank lo, Rank hi); //轴点构造算法
	void quickSort(Rank lo, Rank hi); //快速排序算法
	void heapSort(Rank lo, Rank hi); //堆排序(稍后结合完全堆讲解)
public:

//构造函数
	Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) //容量为c、规模为s、所有元素初始为v
	{
	elem = new T[capacity = c]; for (size = 0; size < s; elem[size++] = v);
	}//s<=c
	Vector(const T* A, Rank n) { copyFrom(A, 0, n); } //数组整体复制
	Vector(const T* A, Rank lo, Rank hi) { copyFrom(A, lo, hi); }//区间
	Vector(Vector<T> const& V) { copyFrom(V.elem, 0, V.size); } //向量整体复制
	Vector(Vector<T> const& V, Rank lo, Rank hi) { copyFrom(V.elem, lo, hi); }//区间
// 析构函数
	~Vector() { delete[] elem; } //释放内部空间
	// 只读访问接口
	Rank getSize() const { return getSize; } //规模
	bool empty() const { return !getSize; } //判空
	int disordered() const{//判断向量是否已排序
	    int n=0;
	    for(int i=1;i<size;i++)//逐一检查size-1对相邻元素
		 if(elem[i-1]>elem[i]) n++;//逆序则计数
		return n; //向量有序当且仅当n=0
    }
	Rank find(T const& e) const { return find(e, 0, size); }//无序向量整体查找
	Rank find(T const& e, Rank lo, Rank hi) const{//assert:0<=lo < hi <=size 
       while ((lo<hi--)&&(e !=elem[hi]));//从后向前，顺序查找
       return hi;//若hi<lo,则意味着失败；否则hi即命中元素的秩
	}//无序向量区间查找
	Rank search(T const& e) const //有序向量整体查找
	{
		return (0 >= size) ? -1 : search(e, 0, size);}
	Rank search(T const& e, Rank lo, Rank hi) const{//assert:0 <=lo <hi <=size 
	   return(rand()%2)?//按各50%的概率随机使用二分查找或Fibonacci查找
            binSearch(elem, e, lo, hi):fibSearch(elem, e, lo,hi ); //有序向量区间查找
	   } // 可写访问接口
	T& operator[] (Rank r) const{//重载下标操作符
		return elem[r];}// assert:O<=r<size//重载下标操作符, 可以类似于数组形式引用各元素
	Vector<T>& operator= (Vector<T> const&V){//重载赋值操作符, 以便直接克隆向量
		if(this != &V)delete []elem;//释放原有内容
        copyFrom(V.elem, 0,V.size());//整体复制
		return*this;}//返回当前对象的引用，以便链式赋值
	T remove(Rank r){//删除向量中秩为r的元素，0<= r<size 
        T e=elem[r];//备份被删除元素
        remove(r,r+1);//调用区间删除算法，等效于对区间[r, r+1)的删除
		return e;}//返回被删除元素
		//删除秩为r的元素
	int remove(Rank lo, Rank hi){
		if(lo== hi)return 0;//出于效率考虑，单独处理退化情况，比如remove(0, 0) 
        while (hi<size)elem[lo++]=elem[hi++];//[hi,size)顺次前移hi-lo个单元
        size=lo;//更新规模，直接丢弃尾部[lo,size=hi)区间
        shrink();//若有必要，则缩容 
        return  hi-lo;}//返回被册元素的
		//删除秩在区间[ lo, hi)之内的元素
	Rank insert(Rank r, T const& e){//assert:0<=r<= size 
	    if (size >= capacity)  // 如果插入位置超出当前大小，插入到末尾
            expand();//若有必要，扩容
        for (Rank i=size;i>r;i--)elem[i]=elem[i-1];//自后向前，后继元素顺次后移一个单元
           elem[r]=e;size++;//置入新元素并更新容量
           return r;//返回秩
	    }
		 //插入元素//将e作为秩为元素插入
	Rank insert(T const& e) { return insert(size, e); } //默认作为末元素插入
	void sort(Rank lo, Rank hi){ //向量区间[1o,hi)排序
	   switch(rand()%5){//随机选取排序算法。可根据具体问题的特点灵活选取或扩充
          case 1: bubbleSort (lo,hi);break;//起泡排序
          case 2: selectionSort(lo, hi);break;//选择排序(习题) 
		  case 3:mergeSort (lo, hi);break; //归并排序
          case 4:heapSort(lo,hi );break;//堆排序(稍后介绍) 
		  default:quickSort(lo,hi);break; //快速排序(稍后介绍) //对[ lo, hi)排序
	}
}
	void sort() { sort(0, size); } //整体排序
	void unsort(Rank lo, Rank hi){//等概率随机置乱区间 [lo,hi) 
	   T*V=elem+10;//将子向量elem[lo, hi)视作另一向量V[e, hi-1o) 
       for(Rank i= hi-lo;i>0;i--)//自后向前
         swap(V[i-1],V[rand()%i]);//将V[i-1]与V[e,1)中某一元素随机交换
	}//对[ lo, hi)置乱
	void unsort() { unsort(0, size); } //整体置乱
	int deduplicate(){ //删除无序向量中重复元素(高效版) 
          if (size <= 1) return 0; // 如果元素数量小于等于1，则不需要去重。
    int oldsize = size; // 记录原始大小。
    Rank i = 0; // 初始化指针i，用于遍历。
    for (Rank j = 1; j < oldsize; ++j) {
        if (elem[i] != elem[j]) { // 如果当前元素与下一个元素不同，
            elem[++i] = elem[j]; // 则将下一个元素复制到i的位置，并将i向前移动。
        }
    }
    size = i + 1; // 更新size为去重后的大小。
    shrink(); // 如果需要，缩小容量。
    return oldsize - size; // 返回删除的元素数量。
    }
	int uniquify(){//有序向量重复元素剔除算法(高效版) 
       Rank i=0,j=0;//各对互异“相邻”元素的秩
       while(++j<size)//逐一扫描，直至末元素
          if(elem[i]!=elem[j])//跳过雷同者
            elem[++i]=elem[j];//发现不同元素时，向前移至紧邻于前者右侧
       size=++i;shrink();//直接截除尾部多余元素
       return j-i;//向量规模变化量，即被删除元素总数//有序去重
	}//遍历
	void traverse(void (*visit) (T&)){//借助函数指针机制
		for (int i=0;i<size;i++)visit(elem[i]);}//遍历向量
		//遍历(使用函数指针, 只读或局部性修改
	template < typename VST> void traverse(VST&vistor){//借助函数对象机制
        for(int i=0;i<size; i++)visitor (elem[i]);}//遍历向量
		//遍历(使用函数对象, 可全局性修改)
		Vector<T> intervalSearch(double m1, double m2) const;
	};//Vector
	template <typename T>
Vector<T> Vector<T>::intervalSearch(double m1, double m2) const
{
    Vector<T> result;
    for (Rank i = 0; i < size; i++)
    {
        double modulus = elem[i].modulus();
        if (modulus >= m1 && modulus < m2)
        {
            result.insert(result.size(), elem[i]);
        }
    }
    return result;
}
	
	// 主函数，用于测试 Vector 类
    int main() {
    srand(time(0)); // 初始化随机数生成器

    // 创建一个复数 Vector 实例
    Vector<Complex> complexVec;
    for (int i = 0; i < 20; ++i) {
        complexVec.insert(complexVec.getSize(), Complex(rand() % 100, rand() % 100));
    }

    // 测试置乱

    // 使用 unsort 函数置乱向量
    complexVec.unsort(0, complexVec.getSize() - 1);

    for (int i = 0; i < complexVec.getSize(); ++i) {
        cout << complexVec[i].real << " + " << complexVec[i].imag << "i" << endl;
    }

    // 测试查找
    Complex target(25, 25);
    int foundIndex = complexVec.find(target, 0, complexVec.getSize());
    if (foundIndex != -1) {
        cout << "Found: " << complexVec[foundIndex].real << " + " << complexVec[foundIndex].imag << "i" << endl;
    } else {
        cout << "Not found" << endl;
    }

    // 测试插入
    complexVec.insert(10, Complex(10, 10));
    cout << "After insertion:" << endl;
    for (int i = 0; i < complexVec.getSize(); ++i) {
        cout << complexVec[i].real << " + " << complexVec[i].imag << "i" << endl;
    }

    // 测试删除
    if (foundIndex != -1) {
        complexVec.remove(foundIndex);
    }
    cout << "After deletion:" << endl;
    for (int i = 0; i < complexVec.getSize(); ++i) {
        cout << complexVec[i].real << " + " << complexVec[i].imag << "i" << endl;
    }

    // 测试唯一化
    complexVec.deduplicate();
    cout << "After uniquifying:" << endl;
    for (int i = 0; i < complexVec.getSize(); ++i) {
        cout << complexVec[i].real << " + " << complexVec[i].imag << "i" << endl;
    }

    // 测试排序
    clock_t start = clock();
    complexVec.sort();
    clock_t end = clock();
    cout << "Sorted vector:" << endl;
    for (int i = 0; i < complexVec.getSize(); ++i) {
        cout << complexVec[i].real << " + " << complexVec[i].imag << "i" << endl;
    }
    cout << "Sort time: " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    // 测试区间查找
//    double m1 = 50;
//    double m2 = 70;
//    Vector<Complex> result = intervalSearch(complexVec, m1, m2);
//    cout << "Interval search results:" << endl;
//    for (int i = 0; i < result.getSize(); ++i) {
//        cout << result[i].real << " + " << result[i].imag << "i" << endl;
//    }
Vector<Complex> result = complexVec.intervalSearch(30, 50);
    cout << "Range Search Result [30, 50): ";

    return 0;
}
