/*
 * $Id: $
 * $Copyright:.$
 *
 * Broadcom autocomplete
 */


#ifdef INCLUDE_AUTOCOMPLETE

#include <sal/appl/editline/autocomplete.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>
#include <sal/core/alloc.h>
#include <soc/drv.h>
#include <sys/ioctl.h>

char autocomplete_string[AUTOCOMPLETE_MAX_UNITS][AUTOCOMPLETE_MAX_STRING_LEN];

static autocomplete_node_t autocomplete_root[AUTOCOMPLETE_MAX_UNITS];


/* find the first attached unit. if there are no attached units, return -1 */

static int first_attached_unit(void) {

    int first;
    for (first = 0; first < soc_ndev; first++) {
        if (soc_attached(SOC_NDEV_IDX2DEV(first))) {
            return SOC_NDEV_IDX2DEV(first);
        }
    }
    return -1;

}


autocomplete_node_t *autocomplete_find_root(int unit, char *keyword) {
    
    autocomplete_node_t *node = autocomplete_root[unit].first_child;
    
    while (node != NULL) {
        if (sal_strcasecmp(node->keyword, keyword) == 0) {
            return node;
        }
        node = node->next_sibling;
    }
    
    return NULL;
    
}


/* for description see the header include/sal/appl/editline/autocomplete.h */

autocomplete_node_t *autocomplete_node_add(int unit, autocomplete_node_t *parent, char *keyword) {

    autocomplete_node_t *node = (autocomplete_node_t*) sal_alloc(sizeof(*node), "autocomplete node");

    node->keyword = sal_strdup(keyword);
    node->first_child = NULL;
    node->next_sibling = NULL;
    node->parent = parent;

    /* check if parent is root */
    if (parent == NULL) {
        parent = &autocomplete_root[unit];
        node->parent = parent;
    }

    /* check if it should be the first child of parent */
    if (parent->first_child == NULL) {
        parent->first_child = node;
    } else {
        autocomplete_node_t* last_child = parent->first_child;
        while(last_child->next_sibling != NULL) {
            last_child = last_child->next_sibling;
        }
        last_child->next_sibling = node;
    }

    return node;
}

/* for description see the header include/sal/appl/editline/autocomplete.h */

void autocomplete_delete_all(int unit) {

    autocomplete_node_t *node = autocomplete_root[unit].first_child;
    autocomplete_node_t *next_node;
    while (node != NULL) {
        next_node = node->next_sibling;
        autocomplete_node_delete(unit, node);
        node = next_node;
    }
    autocomplete_root[unit].first_child = NULL;

}

/* for description see the header include/sal/appl/editline/autocomplete.h */

void autocomplete_node_delete(int unit, autocomplete_node_t* node) {

    autocomplete_node_t *parent;
    autocomplete_node_t *child;

    if (node == NULL)
        return;

    parent = node->parent;
    child = node->first_child;

    /* check if the node is the first child of it's parent */
    if (parent->first_child == node) {
        parent->first_child = node->next_sibling;
    } else {
        autocomplete_node_t* older_sibling = parent->first_child;
        while(older_sibling->next_sibling != node) {
            older_sibling = older_sibling->next_sibling;
        }
        older_sibling->next_sibling = node->next_sibling;
    }

    /* delete all of the node's children */
    while (child != NULL) {
        autocomplete_node_t* next_child = child->next_sibling;
        autocomplete_node_delete(unit, child);
        child = next_child;
    }

    /* delete the node */
    sal_free(node->keyword);
    sal_free(node);

    return;
}

/* get a prefix as a parameter and a new word, calcuate the largest common prefix and save it in prefix */

static void update_common_prefix(int index, char *prefix, char *new_word) {
    /* if the new_word is the first word */
    if (index == 0) {
        sal_strncpy(prefix, new_word, AUTOCOMPLETE_MAX_STRING_LEN-1);
    } else {
        int i=0;
        while (prefix[i] == new_word[i]) {
            i++;
        }
        /* truncate the prefix */
        sal_memset(prefix+i, 0, sal_strlen(prefix+i));
    }
}

/* for description see the header include/sal/appl/editline/autocomplete.h */

char* autocomplete_print(char* input, char* prompt) {
   autocomplete_node_t *prev_node = NULL;
   autocomplete_node_t *node = NULL;
   char new_line[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char str_to_print[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char str_tmp[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char common_prefix[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   char *prefix;
   const char s[2] = " ";
   char *token;
   int token_idx=0;
   int node_depth=0;
   int num_of_suggestions = 0;
   char str_cpy[AUTOCOMPLETE_MAX_STRING_LEN] = {0};
   int max_autocomplete_str_len = 0;
   int accumulative_line_len = 0;
   struct winsize window_size;
   int window_len = 0;
   int unit;

   /* get the first attached unit, and use the autocomplete tree of this unit */
   unit = first_attached_unit();

   if (unit == -1) {
       sal_printf("\nError: autocomplete failed. No attached unit found.\n");
       sal_printf("%s", prompt);
       return "";
   }

   node = prev_node = autocomplete_root[unit].first_child;

   if (node == NULL) {
       return "";
   }

   sal_strncpy(str_cpy, input, AUTOCOMPLETE_MAX_STRING_LEN-1);

   /* get the first token */
   token = strtok(str_cpy, s);

   /* walk through other tokens */
   while( token != NULL )
   {
      token_idx++;
      prev_node = node;
      while(node != NULL && node->keyword != NULL) {
          if (sal_strcasecmp(node->keyword, token) == 0) {
              node = node->first_child;
              node_depth++;
              break;
          }
          node = node->next_sibling;
      }
      if (token_idx != node_depth) break;
      sal_strncat(new_line,token, AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(new_line) - 1);
      sal_strncat(new_line," ", AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(new_line) -1);
      token = strtok(NULL, s);
   }

   /* no suggestions */
   if (token_idx != node_depth && strtok(NULL, s)) return "";
   /* show all options */
   if (token_idx == node_depth) {
       prefix = "";
   } else { /* complete by the prefix */
       node = prev_node;
       prefix = token;
   }

   /* walk through the suggestions */
   while(node != NULL && node->keyword != NULL) {
       if (sal_strncasecmp(node->keyword, prefix, sal_strlen(prefix)) == 0) {
           update_common_prefix(num_of_suggestions, common_prefix, node->keyword);
           sal_sprintf(str_tmp, "%s ", node->keyword);
           if (max_autocomplete_str_len < sal_strlen(str_tmp)) {
               max_autocomplete_str_len = sal_strlen(str_tmp);
           }
           sal_strncat(str_to_print, str_tmp, AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(str_to_print)-1);
           sal_strncat(new_line,str_tmp, AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(new_line)-1);
           num_of_suggestions++;
       }
       node = node->next_sibling;
   }

   sal_strcpy(autocomplete_string[unit],"");

   if (num_of_suggestions > 0 && token_idx == node_depth && sal_strlen(input) > 0 && input[sal_strlen(input)-1] != ' ') {
       sal_strncat(autocomplete_string[unit]," ", AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(autocomplete_string[unit]) - 1);
   }

   if (num_of_suggestions > 1) {
       ioctl(0, TIOCGWINSZ, &window_size);
       window_len = window_size.ws_col;
       sal_printf("\n");
       token = strtok(str_to_print, s);
       while( token != NULL )
       {
           accumulative_line_len += max_autocomplete_str_len;
           if (accumulative_line_len >= window_len) {
               sal_printf("\n");
               accumulative_line_len = max_autocomplete_str_len;
           }
           sal_printf("%*s", -max_autocomplete_str_len, token);
           token = strtok(NULL, s);
       }
       sal_printf("\n");
       sal_printf("%s%s", prompt, input);
   }
   sal_strncat(autocomplete_string[unit], common_prefix + sal_strlen(prefix), AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(autocomplete_string[unit]) - 1);

   return autocomplete_string[unit];
}

#else /* INCLUDE_AUTOCOMPLETE */
int _autocomplete_autocomplete_not_empty;
#endif /* INCLUDE_AUTOCOMPLETE */
