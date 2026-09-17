#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <string.h>


#include <stdio.h>

typedef struct FDict FDict;
int fdict_open(FDict* dict, char* filepath, int flags, int line_len);
int fdict_close(FDict* dict);
int fdict_find(const FDict* dict, const char* word, int* out);
int fdict_find_helper(
    const FDict* dict, 
    const char* word, 
    ssize_t start, 
    ssize_t end, 
    int* out
    );
int fdict_get_line(const FDict* dict, size_t line, char* buf);
void fdict_pad_word(const char* str, size_t n, char buf[n]);

struct FDict {
  int descriptor;
  size_t line_len;
  off_t file_len; 
  size_t num_lines;
};

static inline int print_errno() {
  int err = errno;
  
  char* err_str = strerror(err);
  write(STDERR_FILENO, err_str, strlen(err_str));
  write(STDERR_FILENO, "\n", 1);

  return err;
}

/*
 * @brief Opens the given dictionary file
 *
 * @param dict The dictionary to write to
 * @param filepath The filepath of the given file dictionary
 * @param flags The flags to open with
 * @param line_len The length of each dictionary line
 * 
 * @return 0 on success, error number on failure
 * */
int fdict_open(FDict* dict, char* filepath, int flags, int line_len) {
  if(!dict || line_len < 1)
    return -1;

  int fd = open(filepath, flags);

  if(fd < 0)
    return print_errno();
  
  off_t file_len = lseek(fd, 0, SEEK_END);
  
  if(file_len == (off_t) -1) {  
    int err = print_errno();
    fdict_close(dict);
  
    return err;
  }
  
  *dict = (FDict) {
    .descriptor = fd,
    .line_len = line_len,
    .file_len = file_len,
    .num_lines = file_len / line_len
  };
  
  return 0;
}

/*
 * @brief Closes the supplied dictionary
 *
 * @param dict The dictionary to close
 *
 * @return 0 on success, error number on failure
 * */
int fdict_close(FDict* dict) {
  int err = close(dict->descriptor);

  if(err)
    return print_errno();

  dict->line_len = 0;
  dict->file_len = 0;

  return 0;
}

/*
 * @brief Searches the dictionary for the supplied word using binary search
 *
 * @param dict The file dictionary to search
 * @param word The word to search for
 * @param out A pointer to an int where the line number will be stored
 *
 * @return 0 on success, error number on failure
 * */
int fdict_find(const FDict* dict, const char* word, int* out) {
  if(!dict || !word)
    return -1;
  
  char padded[dict->line_len];
  fdict_pad_word(word, dict->line_len, padded);

  return fdict_find_helper(dict, padded, 0, dict->num_lines - 1, out);
}

int fdict_find_helper(
    const FDict* dict, 
    const char* word, 
    ssize_t start, 
    ssize_t end,
    int* out
) {
  if(end < start) {
    return 0;
  }

  ssize_t mid = (end + start) / 2;
  
  char curr_word[dict->line_len]; 
  int err = fdict_get_line(dict, mid, curr_word);
  
  if(err)
    return err;

  int cmp = strcmp(word, curr_word);

  if(cmp == 0) {
    *out = mid + 1;
    return 0;
  }
  
  if(start == end)
    *out = -(start + 1);

  if(cmp < 0)
    return fdict_find_helper(dict, word, start, mid - 1, out);
  else
    return fdict_find_helper(dict, word, mid + 1, end, out);
}

/*
 * @brief Retrieves a line from the provided file dictionary and converts
 *        it to a null terminated string
 *
 * @param dict The dictionary to use 
 * @param line The line number
 * @param buf The buffer to write to, size dict->line_len
 *
 * @return 0 on success, error number on failure
 * */
int fdict_get_line(const FDict* dict, size_t line, char* buf) {
  off_t off = lseek(dict->descriptor, line * dict->line_len, SEEK_SET);
  if(off == (off_t) -1)
    return print_errno();
  
  ssize_t bytes = read(dict->descriptor, buf, dict->line_len);

  if(bytes == -1)
    return print_errno();
  
  buf[dict->line_len - 1] = '\0';
  return 0;
}

/*
 * @brief Pads a string with spaces and adds a null terminator
 *
 * @param str The string to pad
 * @param n The length of the padded string
 * @param buf The output buffer, size n
 * 
 * */
void fdict_pad_word(const char* str, size_t n, char buf[n]) {
  size_t word_len = strlen(str);
  for(size_t i = 0; i < n - 1; i++)
    buf[i] = i >= word_len ? ' ' : str[i];

  buf[n - 1] = '\0';
}

int lineNum(char *dictionaryName, char *word, int length) {
  FDict dict;
  int open_err = fdict_open(&dict, dictionaryName, O_RDONLY, length);

  if(open_err)
    return open_err;
  
  int line;
  int find_err = fdict_find(&dict, word, &line);
  
  int close_err = fdict_close(&dict);

  if(find_err)
    return find_err;

  if(close_err)
    return close_err;

  return line;
}

