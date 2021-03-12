#include "structDefinitions.h"
#include "functionDefinitions.h"
#include "stackFunctions.h"
#include "queueFunctions.h"

int main(int argc, char* argv[]) {
    TBrowser browser;
    char * command;
    char * buffer;
    char * parameter;

    browser = Init_Browser();
    set_band(browser, "1024");
    newtab(browser);
    
    /* Open the input file given as parameter */
    FILE *input = fopen(argv[1], "r");

    /* Calculate the size of the input file */
    fseek (input, 0, SEEK_END);        
    long input_size = ftell (input);
    rewind (input);

    buffer = (char *) calloc(input_size, sizeof(char));
    if (!buffer) return 0;

    /* Move the contains of the input file into the buffer */
    fread (buffer, 1, input_size, input);

    /* Close the input file */
    fclose (input);

    /* Open the output file, given as the "second" argument, in write mode 
       and write in it everything that's written to stdout. */
    freopen (argv[2], "w", stdout);

    command = strtok(buffer, " \n");
    
    while (command != NULL) {
        if (!strcmp(command, "newtab")) {
            newtab(browser);
        } else if (!strcmp(command, "set_band")) {
            command = strtok (NULL, " \n");
            parameter = command;

            set_band(browser, parameter);
        } else if (!strcmp(command, "deltab")) {
            deltab(browser);
        } else if (!strcmp(command, "change_tab")) {
            command = strtok (NULL, " \n");
            parameter = command;

            change_tab(browser, parameter);
        } else if (!strcmp(command, "print_open_tabs")) {
            print_open_tabs(browser);
        } else if (!strcmp(command, "history")) {
            history(browser);
        } else if (!strcmp(command, "goto")) {
            command = strtok (NULL, " \n");
            parameter = command;

            go_to(browser, parameter);
            if (!EMPTY_QUEUE(browser->download_priority) && 
                browser->current_tab != NULL &&
                browser->current_tab->current_page != NULL) {
                wait(browser, "1");
            }
        } else if (!strcmp(command, "del_history")) {
            command = strtok (NULL, " \n");
            parameter = command;

            del_history(browser, parameter);
        } else if (!strcmp(command, "back")) {
            back(browser);
        } else if (!strcmp(command, "forward")) {
            forward(browser);
        } else if (!strcmp(command, "list_dl")) {
            list_dl(browser);
        } else if (!strcmp(command, "downloads")) {
            downloads(browser); 
        } else if (!strcmp(command, "download")) {
            command = strtok (NULL, " \n");
            parameter = command;

            download(browser, parameter);
        } else if (!strcmp(command, "wait")) {
            command = strtok (NULL, " \n");
            parameter = command;

            wait(browser, parameter);
        }
        
        command = strtok (NULL, " \n");
    }

    fclose(stdout);

    return 0;
}