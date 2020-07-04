#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

//functie care imi sterge ultimul
//caracter dintr-un sir
//O folosesc pentru a sterge un enter-ul
// de la sfarsitul unor comenzi
void strdel(char *str)
{
	char aux[200];

	strncpy(aux, str, strlen(str) - 1);
	aux[strlen(str) - 1] = '\0';
	strcpy(str, aux);
}

//Functie care initializeaza radacina(root)
void init_root(Directory **root)
{
	(*root) = (Directory *)malloc(sizeof(Directory));
	(*root)->name = (char *)malloc(2 * sizeof(char));
	strcpy((*root)->name, "");
	(*root)->parentDir = NULL;
	(*root)->flist = NULL;
	(*root)->dlist = NULL;
}

//Functie care imi pune in ordine lexicografica
//elementele in lista de fisiere dintr-un director
void push_front_File(Directory **head, File_List **new_node, char *str, char *p)
{
	*new_node = (File_List *)malloc(sizeof(File_List));
	(*new_node)->prev = NULL;
	(*new_node)->next = NULL;
	(*new_node)->file = (File *)malloc(sizeof(File));
	(*new_node)->file->data =
	(char *)malloc((strlen(p + 1) + 1) * sizeof(char));
	char aux[100];
	char *p1, *p2, *p3;

	strcpy(aux, str);
	p1 = strtok(aux, " ");
	p2 = strtok(NULL, " ");
	(*new_node)->file->name =
	(char *)malloc((strlen(p2) + 1) * sizeof(char));
	strcpy((*new_node)->file->name, p2);
	p3 = strtok(NULL, " ");
	strcpy((*new_node)->file->data, p3);
	(*new_node)->file->size = strlen((*new_node)->file->data);
	if ((*head)->flist == NULL) {
		(*new_node)->next = (*head)->flist;
		(*new_node)->prev = NULL;
		(*head)->flist = *new_node;
	} else
		if (strcmp((*new_node)->file->name,
			(*head)->flist->file->name) < 0) {
			(*new_node)->next = (*head)->flist;
		(*new_node)->next->prev = *new_node;
			(*new_node)->prev = NULL;
			(*head)->flist = *new_node;
		} else
			if (strcmp((*new_node)->file->name,
				(*head)->flist->file->name) > 0) {
				File_List *it = (*head)->flist;

				while (it->next &&
					strcmp((*new_node)->file->name,
						it->next->file->name) > 0) {
					it = it->next;
				}
				if (it->next == NULL) {
					it->next = *new_node;
					(*new_node)->next = NULL;
					(*new_node)->prev = it;
				} else {
					(*new_node)->next = it->next;
					(*new_node)->prev = it;
					it->next->prev = *new_node;
					it->next = *new_node;
				}
			}
}

//Functie care imi pune in ordine lexicografica
//elementele in lista de directoare a unui director
void push_front_Directory(Directory **head, Directory_List *new_node)
{
	if ((*head)->dlist == NULL) {
		new_node->next = (*head)->dlist;
		new_node->prev = NULL;
		(*head)->dlist = new_node;
	} else
		if (strcmp(new_node->directory->name,
			(*head)->dlist->directory->name) < 0) {
			new_node->next = (*head)->dlist;
			new_node->next->prev = new_node;
			new_node->prev = NULL;
			(*head)->dlist = new_node;
		} else
			if (strcmp(new_node->directory->name,
				(*head)->dlist->directory->name) > 0) {
				Directory_List *it = (*head)->dlist;

				while (it->next &&
					strcmp(new_node->directory->name,
					it->next->directory->name) > 0) {
					it = it->next;
				}
				if (it->next == NULL) {
					it->next = new_node;
					new_node->next = NULL;
					new_node->prev = it;
				} else {
					new_node->next = it->next;
					new_node->prev = it;
					it->next->prev = new_node;
					it->next = new_node;
				}
			}
}

//Functie care executa comanda
//print working directory
void pwd(Directory *wd)
{
	if (wd->parentDir != NULL)
		pwd(wd->parentDir);
	if (wd->parentDir != NULL) {
		printf("/");
		printf("%s", wd->name);
	} else
		printf("%s", wd->name);
}

//Functie recursiva care executa
//comanda "tree"
void tree(Directory_List *wd, File_List *wdf, int i)
{
	Directory_List *it = wd;
	File_List *itf = wdf;

	while (itf != NULL) {
		printf("%*s\n", (int)(i +
			strlen(itf->file->name)), itf->file->name);
		itf = itf->next;
	}
	while (it != NULL) {
		printf("%*s\n", (int)(i +
		strlen(it->directory->name)), it->directory->name);
		tree(it->directory->dlist,
			it->directory->flist, i + 4);
		it = it->next;
	}
}

//functie care elibereaza memoria
//unui fisier
void free_file(File_List **filelist)
{
	free((*filelist)->file->name);
	free((*filelist)->file->data);
	free((*filelist)->file);
	free((*filelist));
}

//functie care elibereaza memoria
//dintr-un director in mod recursiv
void free_dir(Directory_List **dirlist)
{
	Directory_List *it2 = (*dirlist)->directory->dlist;
	File_List *it;

	it = (*dirlist)->directory->flist;
	File_List *aux2;
	Directory_List *aux1;

	while (it2 != NULL) {
		aux1 = it2->next;
		free_dir(&it2);
		it2 = aux1;
	}
	while (it) {
		aux2 = it->next;
		if (it->prev != NULL &&
			it->next != NULL) {
			it->prev->next = it->next;
			it->next->prev = it->prev;
		} else
			if (it->prev == NULL
				&& it->next != NULL) {
				it->next->prev = NULL;
				(*dirlist)->directory->flist = it->next;
			} else
				if (it->next
			== NULL && it->prev != NULL) {
					it->prev->next = NULL;
				} else
					(*dirlist)->directory->flist = NULL;
					free_file(&it);
		it = aux2;
	}
	free((*dirlist)->directory->name);
	free((*dirlist)->directory);
	free((*dirlist));
}

//functie care executa comanda
//"find" in mod recursiv
void find(Directory *wd, char *str, int i,
	int a, int b, int c, char *p4, int ok)
{
	File_List *file_list = wd->flist;

	while (file_list != NULL) {
		if (strstr(file_list->file->data, p4) != NULL
		&& file_list->file->size >= b &&
		file_list->file->size <= c) {
			if (ok == 0) {
				printf("%s", file_list->file->name);
				ok = 1;
		} else
			printf(" %s", file_list->file->name);
		}
		file_list = file_list->next;
	}
	Directory_List *dirlist;

	dirlist = wd->dlist;
	if (i + 1 <= a)
		while (dirlist) {
			find(dirlist->directory, str, i + 1, a, b, c, p4, ok);
			dirlist = dirlist->next;
		}
}

//Functie de meniu
//Aici este o bucla while
//In aceasta se determina care
//comanda va fi executata
//Functiile ls, rm, rmdir, cd
//sunt implementate aici
//rm si rmdir apeleaza si la
//functiile de eliberare de memorie
//Aici am si implementate comenzile
//de create fs si delete fs, create fs
//determinand intrarea in while
void prompt(void)
{
	char p[100];
	char *str;

	str = malloc(200 * sizeof(char));
	fgets(str, 200, stdin);
	strdel(str);
	if (strcmp(str, "create fs") == 0) {
		Directory *root;
		char str1[10], str2[5];
		char str4[10];
		char *p1, *p2, *p3, *p4;
		int a, b, c;

		init_root(&root);
		File_List *new_node_file;
		Directory_List *new_node_directory;
		Directory *wd;

		wd = root;
		while (fgets(str, 200, stdin)) {

			strncpy(str2, str, 2);
			strncpy(str1, str, 5);
			str2[2] = '\0';
			str1[5] = '\0';
			strdel(str);

			if (strcmp(str, "delete fs") == 0)
				break;
			if (strcmp(str1, "touch") == 0) {
				strcpy(p, strchr(str + 6, ' '));
				push_front_File(&wd, &new_node_file, str, p);
			}
			if (strcmp(str1, "mkdir") == 0) {
				new_node_directory =
				malloc(sizeof(Directory_List));
				new_node_directory->directory =
				malloc(sizeof(Directory));
				new_node_directory->directory->name =
				malloc((strlen(str + 6) + 1) * sizeof(char));
				strcpy(new_node_directory->directory->name,
					str + 6);
				new_node_directory->directory->parentDir = wd;
				push_front_Directory(&wd, new_node_directory);
				new_node_directory->directory->flist = NULL;
				new_node_directory->directory->dlist = NULL;
			}
			if (strcmp(str, "ls") == 0) {
				File_List *it_flist = wd->flist;

				while (it_flist != NULL) {
					if (it_flist->next == NULL) {
						printf("%s",
							it_flist->file->name);
						it_flist = it_flist->next;
				} else {
					printf("%s ", it_flist->file->name);
					it_flist = it_flist->next;
				}
			}
			Directory_List *it_dlist;

			it_dlist = wd->dlist;
			if (it_dlist == NULL)
				printf("\n");
			else {
				if (wd->flist != NULL)
					printf(" ");
				while (it_dlist != NULL) {
					if (it_dlist->next == NULL) {
						printf("%s\n",
						it_dlist->directory->name);
						it_dlist = it_dlist->next;
					} else {
						printf("%s ",
						it_dlist->directory->name);
						it_dlist = it_dlist->next;
					}
				}
			}
		}
		if (strcmp(str2, "cd") == 0) {
			int ok = 0;

			strcpy(p, strchr(str, ' '));
			Directory_List *dir = wd->dlist;

			if (strcmp(p + 1, "..") == 0) {
				wd = wd->parentDir;
			} else {
				while (dir != NULL) {
					if (strcmp(dir->directory->name,
						p + 1) == 0) {
						ok = 1;
						wd = dir->directory;
						}
					dir = dir->next;
				}
				if (ok == 0)
					printf(
"Cannot move to '%s': No such directory!\n", p + 1);
			}
		}
		if (strcmp(str, "pwd") == 0) {
			if (wd == root)
				printf("/\n");
			else {
				pwd(wd);
				printf("\n");
			}
		}
		if (strcmp(str, "tree") == 0) {
			int i = 4;
			Directory_List *it = wd->dlist;
			File_List *itf = wd->flist;

			if (wd == root)
				printf("%s\n", "/");
			else
				printf("%s\n", wd->name);
			tree(it, itf, i);
		}
		if (strncmp(str, "rm", 2) == 0 &&
			strncmp(str, "rmdir", 5) != 0) {
			int ok1 = 0;

			strcpy(p, strchr(str, ' '));
			File_List *it = wd->flist;
	while (it != NULL) {
		if (strcmp(it->file->name,
			p + 1) == 0) {
			ok1 = 1;
			if (it->prev != NULL &&
				it->next != NULL) {
				it->prev->next = it->next;
				it->next->prev = it->prev;
			} else
				if (it->prev == NULL
					&& it->next != NULL) {
					it->next->prev = NULL;
					wd->flist = it->next;
				} else
					if (it->next
				== NULL && it->prev != NULL) {
						it->prev->next = NULL;
					} else
						wd->flist = NULL;
						free_file(&it);
			break;
		}
		it = it->next;
	}
	if (ok1 == 0)
		printf(
"Cannot remove '%s': No such file!\n", p + 1);
}
		if (strncmp(str, "rmdir", 5) == 0) {
			int ok = 0;

			strcpy(p, strchr(str, ' '));
			Directory_List *it = wd->dlist;

	while (it != NULL) {
		if (strcmp(it->directory->name,
			p + 1) == 0) {
			ok = 1;
			if (it->prev != NULL &&
				it->next != NULL) {
				it->prev->next = it->next;
				it->next->prev = it->prev;
			} else
				if (it->prev
				== NULL && it->next != NULL) {
					it->next->prev = NULL;
					wd->dlist = it->next;
				} else
					if (it->next == NULL
						&& it->prev != NULL) {
						it->prev->next = NULL;
					} else
						wd->dlist = NULL;
						free_dir(&it);
			break;
		}
		it = it->next;
	}
	if (ok == 0)
		printf(
"Cannot remove '%s': No such directory!\n", p + 1);
}
	strncpy(str4, str, 4);
	str4[4] = '\0';
	if (strcmp(str4, "find") == 0) {
		p1 = strtok(str, " ");
		p1 = strtok(NULL, " ");
		p2 = strtok(NULL, " ");
		p3 = strtok(NULL, " ");
		p4 = strtok(NULL, " ");
		a = atoi(p1);
		b = atoi(p2);
		c = atoi(p3);
		find(wd, str, 0, a, b, c, p4, 0);
		printf("\n");
	}

	}
		free(root->name);
		free(root);
	}
	free(str);
}

//Functia main
int main(void)
{
	prompt();
	return 0;
}
