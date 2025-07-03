int complex_nested(int x) {
    int result;
    result = 0;
    
    if (x > 0) {
        int i;
        i = 0;
        while (i < x) {
            if (i == 5) {
                result = result + 10;
            } else {
                if (i == 3) {
                    result = result + 5;
                } else {
                    result = result + 1;
                }
            }
            i = i + 1;
        }
    }
    
    return result;
}
