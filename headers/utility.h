/** Searches in str for the string original and replaces it with "replace" */
char *replaceCharacter(char *str, char *original, char *replace)
{
    static char buffer[2048];
    char *p;

    if(!(p = strstr(str, original)))  // Is 'original' even in 'str'?
        return str;

    strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'original' st$
    buffer[p-str] = '\0';

    sprintf(buffer+(p-str), "%s%s", replace, p+strlen(original));

    return buffer;
}

/**Counts the number of pipes in a string*/
int countPipes(char *userInput) {
    int i = 0, count = 0;

    while(*(userInput + i) != '\0') {
        if( *(userInput + i) == '|')
            count++;

        i++;
    }

    return count;
}
