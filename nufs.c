// based on cs3650 starter code

// based on cs3650 starter code

#include <assert.h>
#include <bsd/string.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>

// implementation for: man 2 access
// Checks if a file exists.
int nufs_access(const char *path, int mask) {
  int rv = 0;

  // Only these paths are accessible
  if (strcmp(path, "/") == 0 ||
      strcmp(path, "/myworld.txt") == 0 ||
      strcmp(path, "/test.txt") == 0 ||
      strcmp(path, "/subdir") == 0 ||
      strcmp(path, "/subdir/test2.txt") == 0) {
    rv = 0;
  } else { // ...others do not exist
    rv = -ENOENT;
  }

  printf("access(%s, %04o) -> %d\n", path, mask, rv);
  return rv;
}

// Gets an object's attributes (type, permissions, size, etc).
// Implementation for: man 2 stat
// This is a crucial function.
int nufs_getattr(const char *path, struct stat *st) {
  int rv = 0;

  // Root directory metadata
  if (strcmp(path, "/") == 0) {
    st->st_mode = 040755; // directory
    st->st_size = 0;
    st->st_uid = getuid();
  }
  // Simulated file: /myworld.txt
  else if (strcmp(path, "/myworld.txt") == 0) {
    st->st_mode = 0100644; // regular file
    st->st_size = 8;       // size of "myworld"
    st->st_uid = getuid();
  }
  // Simulated file: /test.txt
  else if (strcmp(path, "/test.txt") == 0) {
    st->st_mode = 0100644; // regular file
    st->st_size = 10;      // size of "0123456789"
    st->st_uid = getuid();
  }
  // Simulated directory: /subdir
  else if (strcmp(path, "/subdir") == 0) {
    st->st_mode = 040755; // directory
    st->st_size = 0;
    st->st_uid = getuid();
  }
  // Simulated file: /subdir/test2.txt
  else if (strcmp(path, "/subdir/test2.txt") == 0) {
    st->st_mode = 0100644; // regular file
    st->st_size = 19;      // size of "Test file in subdir"
    st->st_uid = getuid();
  } else { // Other files do not exist
    rv = -ENOENT;
  }

  printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode,
         st->st_size);
  return rv;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
  struct stat st;
  int rv;

  if (strcmp(path, "/") == 0) {
    // Root directory contents
    rv = nufs_getattr("/", &st);
    assert(rv == 0);
    filler(buf, ".", &st, 0);

    rv = nufs_getattr("/myworld.txt", &st);
    assert(rv == 0);
    filler(buf, "myworld.txt", &st, 0);

    rv = nufs_getattr("/test.txt", &st);
    assert(rv == 0);
    filler(buf, "test.txt", &st, 0);

    rv = nufs_getattr("/subdir", &st);
    assert(rv == 0);
    filler(buf, "subdir", &st, 0);
  } else if (strcmp(path, "/subdir") == 0) {
    // Contents of /subdir
    rv = nufs_getattr("/subdir", &st);
    assert(rv == 0);
    filler(buf, ".", &st, 0);

    rv = nufs_getattr("/subdir/test2.txt", &st);
    assert(rv == 0);
    filler(buf, "test2.txt", &st, 0);
  } else {
    rv = -ENOENT;
  }

  printf("readdir(%s) -> %d\n", path, rv);
  return rv;
}

// Read data from a simulated file
int nufs_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
  int rv = 0;

  if (strcmp(path, "/myworld.txt") == 0) {
    const char *data = "myworld";
    size_t len = strlen(data);
    if (offset < len) {
      if (offset + size > len) {
        size = len - offset;
      }
      memcpy(buf, data + offset, size);
      rv = size;
    }
  } else if (strcmp(path, "/test.txt") == 0) {
    const char *data = "0123456789";
    size_t len = strlen(data);
    if (offset < len) {
      if (offset + size > len) {
        size = len - offset;
      }
      memcpy(buf, data + offset, size);
      rv = size;
    }
  } else if (strcmp(path, "/subdir/test2.txt") == 0) {
    const char *data = "Test file in subdir";
    size_t len = strlen(data);
    if (offset < len) {
      if (offset + size > len) {
        size = len - offset;
      }
      memcpy(buf, data + offset, size);
      rv = size;
    }
  } else {
    rv = -ENOENT;
  }

  printf("read(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
  return rv;
}

// Initialize FUSE operations
void nufs_init_ops(struct fuse_operations *ops) {
  memset(ops, 0, sizeof(struct fuse_operations));
  ops->access = nufs_access;
  ops->getattr = nufs_getattr;
  ops->readdir = nufs_readdir;
  ops->read = nufs_read;
}

// Main function
int main(int argc, char *argv[]) {
  assert(argc > 2 && argc < 6);
  printf("TODO: mount %s as data file\n", argv[--argc]);
  nufs_init_ops(&nufs_ops);
  return fuse_main(argc, argv, &nufs_ops, NULL);
}
