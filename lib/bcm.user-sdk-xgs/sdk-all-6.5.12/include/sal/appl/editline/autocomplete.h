/*
 * $Id: $
 * $Copyright:.$
 *
 * Broadcom autocomplete
 */

#ifndef _INCLUDE_AUTOCOMPLETE_H
#define _INCLUDE_AUTOCOMPLETE_H


#define AUTOCOMPLETE_MAX_STRING_LEN 1024
#define AUTOCOMPLETE_MAX_UNITS 256

typedef struct autocomplete_node_s {
    char *keyword;
    struct autocomplete_node_s *first_child;
    struct autocomplete_node_s *next_sibling;
    struct autocomplete_node_s *parent;
} autocomplete_node_t;

/*
 * use this function to add nodes to the autocomplete mechanism
 * for adding children to this node, use the return value of this function
 * use null for new tree of autocompletion
 *
 * for example, this calls sequence
 *
 *   ac_node1 = autocomplete_node_add(unit, NULL, "kw1");
 *   ac_node2 = autocomplete_node_add(unit, NULL, "kw2");
 *   autocomplete_node_add(unit, ac_node1, "kw11");
 *   autocomplete_node_add(unit, ac_node1, "kw12");
 *   autocomplete_node_add(unit, ac_node2, "kw21");
 *
 *  will create this tree:
 *
 *         root
 *       /     \
 *      /       \
 *     kw1      kw2
 *    /   \       \
 *   kw11  kw12   kw21
 *
 */

autocomplete_node_t *autocomplete_node_add(int unit, autocomplete_node_t *parent, char *keyword);

autocomplete_node_t *autocomplete_find_root(int unit, char *keyword);

/*
 * use this function to remove that has been created by autocomplete_node_add
 * this function remove all of the tree recursively
*/

void autocomplete_node_delete(int unit, autocomplete_node_t *node);

/*
 * use this function to remove the whole tree (per unit) of the autocomplete
 */

void autocomplete_delete_all(int unit);

/*
 * print autocompletion suggestions
 */
char* autocomplete_print(char* input, char* prompt);

#endif /* _INCLUDE_AUTOCOMPLETE_H */
