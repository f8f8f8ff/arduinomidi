#ifndef medianfilter_h
#define medianfilter_h
// must be odd number
const size_t MEDIAN_FILTER_WINDOW_SIZE = 5;

static int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

class median_filter {
    int values[MEDIAN_FILTER_WINDOW_SIZE];
    int median;
    bool changed;

   public:
    void add_reading(int value) {
        // Serial.println(value);
        static int i = 0;
        values[i] = value;
        i += 1;
        if (i >= MEDIAN_FILTER_WINDOW_SIZE) i = 0;
        changed = true;
    }
    int value() {
        // if (!changed) return median;
        int sorted[MEDIAN_FILTER_WINDOW_SIZE];
        for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++) {
            sorted[i] = values[i];
        }
        for (int i = 1; i < MEDIAN_FILTER_WINDOW_SIZE; i++) {
            int key = values[i];
            int j = i - 1;
            while (j >= 0 && sorted[j] > key) {
                sorted[j + 1] = sorted[j];
                j = j - 1;
            }
            sorted[j + 1] = key;
        }
        if (true) {
            Serial.print("median/2 ");
            Serial.print(MEDIAN_FILTER_WINDOW_SIZE / 2);
            for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++) {
                Serial.print(" ");
                Serial.print(sorted[i]);
            }
            Serial.print("\n");
        }
        return sorted[MEDIAN_FILTER_WINDOW_SIZE / 2];
    }
    int last_value() { return median; };
    median_filter() : median(0), changed(false) {
        for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++) {
            values[i] = 0;
        }
    }
};
#endif