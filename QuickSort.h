#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "ISort.h"
#include <stdexcept>
using namespace std;

template<class T>
class QuickSort : public ISort<T> {
private:
    int (*pivotSelection)(T*, int);

public:
    QuickSort(int (*pivotSelection)(T*, int) = 0)
        : pivotSelection(pivotSelection) {}

    void sort(T array[], int size, int (*comparator)(T&, T&) = 0) override {
        if (size<=1) return;
        quickSort(array, 0, size-1, comparator);
    }

private:
    void quickSort(T array[], int left, int right,
                   int (*comparator)(T&, T&) = 0) {
        while (left<right) {
            if (!pivotSelection && EdgeCases(array, left, right, comparator)) return;
            int equalL=partition(array, left, right, comparator);
            T pivot=array[equalL];
            int equalR=equalL;
            while (equalR<right) {
                int compare;
                if (comparator) {
                    compare=comparator(array[equalR+1], pivot);
                }
                else if (array[equalR+1]<pivot) {
                    compare=-1;
                }
                else if (array[equalR+1]>pivot) {
                    compare=1;
                }
                else {
                    compare=0;
                }
                if (compare!=0) break;
                equalR++;
            }
            int leftSide=equalL-left;
            int rightSide=right-equalR;
            if (leftSide<rightSide) {
                quickSort(array, left, equalL-1, comparator);
                left=equalR+1;
            }
            else {
                quickSort(array, equalR+1, right, comparator);
                right=equalL-1;
            }
        }
    }
    bool EdgeCases (T array[], int l, int r, int (*comparator)(T&, T&)) {
        bool ascending=true;
        bool descending=true;
        for (int i=l+1; i<=r; i++) {
            int compare;
            if (comparator) {
                compare=comparator(array[i-1], array[i]);
            }
            else if (array[i-1]<array[i]) {
                compare=-1;
            }
            else if (array[i-1]>array[i]) {
                compare=1;
            }
            else {
                compare=0;
            }
            if (compare<0) descending=false;
            if (compare>0) ascending=false;
        }
        if (ascending) return true;

        if (descending) {
            for (int i=l, j=r; i<j; i++,j--) {
                swap(array[i], array[j]);
            }
            return true;
        }
        return false;
    }
    int partition(T array[], int left, int right,
                  int (*comparator)(T&, T&) = 0) {
        if (pivotSelection) {
            int idx= left + pivotSelection(array+left, right-left+1);
            swap(array[idx], array[right]);
        }
        T pivot=array[right];
        int less= left;
        int current= left;
        int great=right;
        while (current<= great) {
            int compare;
            if (comparator) {
                compare=comparator(array[current],pivot);
            }
            else if (array[current]<pivot) {
                compare=-1;
            }
            else if (array[current]>pivot) {
                compare=1;
            }
            else {
                compare=0;
            }
            if (compare<0) {
                swap(array[less], array[current]);
                less++, current++;
            }
            else if (compare>0) {
                swap (array[great],array[current]);
                great--;
            }
            else {
                current++;
            }
        }
        return less;
    }
};

#endif
