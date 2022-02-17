/**
 * @file utils_impl.h
 * @author Charlemagne Wong (cmasterx.com)
 * @brief Template implementations for utils.h
 * @version 0.1
 * @date 2021-04-06
 */

template<class T>
void Utils::swap(T&a, T&b)
{
    T tmp = static_cast<T&&>(a);
    a = static_cast<T&&>(b);
    b = static_cast<T&&>(tmp);
}

template<class T>
inline T && Utils::move(T &t)
{
    return static_cast<T&&>(t);
}

template<class T>
int Utils::partition(T arr[], int low, int high)
{
    T pivot = arr[high]; // pivot
    int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far
 
    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if (arr[j] < pivot)
        {
            i++; // increment index of smaller element
            Utils::swap(arr[i], arr[j]);
        }
    }
    Utils::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

template<class T>
void Utils::quickSort(T arr[], int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
        at right place */
        int pi = partition(arr, low, high);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
