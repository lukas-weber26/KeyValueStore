#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct key_val {
	int current_length; 
	int max_length;
	int increase_factor; 
	int num_to_increase;
	char ** keys;
	int * vals;
} key_val;

int key_val_add_safe(key_val * k, char * new_key, int new_val) {
	for (int i = 0; i < k->current_length; i++) {
		if (strcmp(k->keys[i], new_key) == 0) {
			return -1;
		}
	}

	int insert_location = k->current_length; 
	if (k->max_length - k->num_to_increase  ==  insert_location) {
		k -> max_length = (int)(k -> max_length * k -> increase_factor); //there are always suss
		k -> keys = realloc(k-> keys, k -> max_length);
		k -> vals = realloc(k-> vals, k -> max_length);
	}

	char * key = malloc(sizeof(char) * (strlen(new_key) + 1));
	strcpy(key, new_key);
	key[strlen(new_key)+1] = '\0';
	k->keys[insert_location] = key; 
	k->vals[insert_location] = new_val; 
	k -> current_length ++;
	return k -> current_length;
}

int key_val_add_unsafe(key_val * k, char * new_key, int new_val) {
	int insert_location = k->current_length; 
	if (k->max_length - k->num_to_increase  ==  insert_location) {
		k -> max_length = (int)(k -> max_length * k -> increase_factor); //there are always suss
		k -> keys = realloc(k-> keys, k -> max_length);
		k -> vals = realloc(k-> vals, k -> max_length);
	}

	char * key = malloc(sizeof(char) * (strlen(new_key) + 1));
	strcpy(key, new_key);
	key[strlen(new_key)+1] = '\0';
	k->keys[insert_location] = key; 
	k->vals[insert_location] = new_val; 
	k -> current_length ++;
}

int key_val_set(key_val * k, char * new_key, int new_val) {
	int insert_location = k->current_length; 
	for (int i = 0; i < k->current_length; i++) {
		if (strcmp(k->keys[i], new_key) == 0) {
			insert_location = i;
			break;
		}
	}
	
	if (insert_location == k -> current_length) {
		if (k->max_length - k->num_to_increase  ==  insert_location) {
			k -> max_length = (int)(k -> max_length * k -> increase_factor); //there are always suss
			k -> keys = realloc(k-> keys, k -> max_length);
			k -> vals = realloc(k-> vals, k -> max_length);
		}

		char * key = malloc(sizeof(char) * (strlen(new_key) + 1));
		strcpy(key, new_key);
		key[strlen(new_key)+1] = '\0';
		k->keys[insert_location] = key; 
		k->vals[insert_location] = new_val; 
		k -> current_length ++;
		return k -> vals[insert_location];
	} else {
		k -> vals[insert_location] = new_val;
		return k -> vals[insert_location];
	}
}

int key_val_get_val(key_val * k, char * key, int * val) {
	for (int i = 0; i < k->current_length; i++) {
		if (strcmp(k->keys[i], key) == 0) {
			*val = k->vals[i];
			return 1;
		}
	}
	return 0;
}

int key_val_remove(key_val * k, char * key) {
	for (int i = 0; i < k->current_length; i++) {
		if (strcmp(k->keys[i], key) == 0) {
			free(k->keys[i]);
			k->keys[i] = malloc(sizeof(char));
			k->keys[i][0] = '\0';
			k -> vals = NULL;
			return 1;
		}
	}
	return -1;
}

void sanatize(key_val * k) {
	int length = k->max_length;
	char ** new_keys = malloc(sizeof(char*) * length);
	int * new_vals = malloc(sizeof(int) * length);
	int new_length = 0; 

	for (int i = 0; i < length; i++) {
		if (strcmp(k->keys[i], "") != 0) {
			new_vals[new_length] = k->vals[i];	
			new_keys[new_length] = k->keys[i];	
			new_length ++; 
		} else {
			free(k->keys[i]);
		}
	}
	
	free(k->vals);
	free(k->keys);

	k -> current_length = new_length;
	k -> max_length = new_length + k -> increase_factor; 
	k -> keys = realloc(new_keys, k->max_length);
	k -> keys = realloc(new_vals, k->max_length);
	
}

int key_val_remove_safe(key_val * k, char * key) {
	int success = key_val_remove(k, key);
	if (success) { 
		sanatize(k);
	}
	return success;
}

void key_val_delete(key_val * k) {
	for (int i =0 ; i < k -> current_length; i++) {
		free(k->keys[i]);
	}
	free(k->keys);
	free(k->vals);
	free(k);
}

int key_val_to_file(key_val * k, char * file_name) {
	FILE * fd; 
	fd = fopen(file_name,"w");
	
	for (int i = 0; i < k->current_length; i++) {
		fprintf(fd,"%s\n", k->keys[i]);	
		fprintf(fd,"%d\n", k->vals[i]);	
	}

	fclose(fd);
	return 1;
}

key_val * key_val_from_file(char * file_name, int increase_factor, int increase_after ) {
	FILE * fd; 
	fd = fopen(file_name, "r");

	key_val * k = malloc(sizeof(key_val));

	k -> current_length = 0;
	k -> max_length = 10+increase_after;
	k -> increase_factor = increase_factor; 
	k -> num_to_increase= increase_after;
	char ** keys = malloc(sizeof(char) * k-> max_length);
	int * vals = malloc(sizeof(char) * k -> max_length);

	char * key;
	unsigned long key_size;
	char * val;
	unsigned long val_size;
	while (getline(&key,&key_size, fd), getline(&val, &val_size, fd)) {
		k -> keys[k -> current_length] = malloc(sizeof(char)*(strlen(key)));
		strcpy(k->keys[k -> current_length], key);
		k->keys[k -> current_length] = '\0';
		k -> vals[k -> current_length] = atoi(val);	
		k -> current_length ++;

		if (k->max_length - k -> num_to_increase <= k-> current_length) {
			k-> max_length += (int)(k->max_length)*(k -> increase_factor); 
			k -> keys = realloc(k->keys, k-> max_length);
			k -> vals = realloc(k->vals, k-> max_length);
		}
	}

	fclose(fd);
	sanatize(k);	
	return k;
}

key_val * key_val_new(int max_length, float increase_factor) {
	key_val * new = malloc(sizeof(key_val));
	new -> current_length = 0;
	new -> max_length = max_length; 
	new -> increase_factor = 2;
	new -> keys = malloc(sizeof(char *) * (max_length));
	new -> vals = malloc(sizeof(int) * (max_length));
	return new; 
}


int main() {
	printf("This is a test\n");
	

	return 0;
}
