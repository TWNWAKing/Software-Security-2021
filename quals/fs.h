#ifndef _FS_H_
#define _FS_H_

#include "list.h"
#include "user.h"
#include <stdint.h>

#define MF_SIZE_INIT 0x100
#define MF_SIZE_MAX  0x1000
#define MF_META_PROT_READ     0b00000001
#define MF_META_PROT_WRITE    0b00000010
#define MF_META_ENCED         0b00000100
#define MF_META_TYPE_IS_DIR   0b00001000
#define MF_META_TYPE_IS_SLINK 0b00010000
#define MF_META_TYPE_IS_HLINK 0b00100000


static int8_t mf_cnt = 0;
MyFile *curr_dir = NULL;
MyFile *softlink = NULL, *hardlink = NULL;

typedef struct iNode
{
    char *content;
    uint8_t refcnt;
} iNode;

typedef struct _MyFile
{
    int8_t fid;
    int8_t uid;
    uint8_t refcnt;
    uint8_t metadata;
    uint16_t size;
    
    char *fn;
    union
    {
        iNode *ino;
        struct _MyFile *link;
    } data;
    
    list_head dir_hd;
    list_head next_file;
} MyFile;

static inline int mf_is_readable(MyFile *mf)
{
    return (mf->metadata & MF_META_PROT_READ) != 0;
}

static inline int mf_is_writable(MyFile *mf)
{
    return (mf->metadata & MF_META_PROT_WRITE) != 0;
}

static inline int mf_is_deleted(MyFile *mf)
{
    return mf->fid == -1;
}

static inline int mf_is_dir(MyFile *mf)
{
    return (mf->metadata & MF_META_TYPE_IS_DIR) != 0;
}

static inline int mf_is_slink(MyFile *mf)
{
    return (mf->metadata & MF_META_TYPE_IS_SLINK) != 0;
}

static inline int mf_is_hlink(MyFile *mf)
{
    return (mf->metadata & MF_META_TYPE_IS_HLINK) != 0;
}

static inline int mf_is_normfile(MyFile *mf)
{
    return ((mf->metadata & MF_META_TYPE_IS_SLINK) |
            (mf->metadata & MF_META_TYPE_IS_HLINK) |
            (mf->metadata & MF_META_TYPE_IS_DIR)) == 0;
}

MyFile *__new_mf();

MyFile *_new_normfile(uint8_t uid, char *fn);
MyFile *_new_dir(uint8_t uid, char *fn);
MyFile *_new_slink(uint8_t uid, MyFile *link, char *fn);
MyFile *_new_hlink(uint8_t uid, MyFile *link, char *fn);

MyFile *_get_mf_by_fname(MyFile *hd, char *fn);

int _release_mf();
int is_desc(MyFile *curr, MyFile *target);
int mf_gc_list_add(GC *gc, list_head *hd);
int get_mf_refcnt(MyFile *mf);

/**
 * create_mf(): create file 
 * > create dir <file_name>
 * > create normfile <file_name>
 */
int create_mf();

/**
 * delete_mf(): delete file
 * > delete <file_name>
 */
int delete_mf();

/**
 * read_mf(): read data from stdin and write to file
 * > read <file_name>
 */
int read_mf(MyUser *ms, MyFile *mf);

/**
 * write_mf(): write file content to stdout
 * > write <file_name>
 */
ssize_t write_mf(MyUser *ms, MyFile *mf);

/**
 * enc_mf(): encrypt file
 * > enc <key> <file_name>
 */
int enc(MyFile *mf, char *key);

/**
 * dec_mf(): decrypt file
 * > dec <key> <file_name>
 */
int dec(MyFile *mf, char *key);

/**
 * set_mf_prot(): set the prot of file
 * > set <prot> <file_name>
 */
void set_mf_prot(MyFile *mf, char *prot);

/**
 * unset_mf_prot(): unset the prot of file
 * > unset <prot> <file_name>
 */
void unset_mf_prot(MyFile *mf, char *prot);

/**
 * show_fileinfo(): show the information of file
 * > show_fileinfo <file_name>
 */
int show_fileinfo(char *fn);
void _show_fileinfo(MyFile *mf);

/**
 * list_file(): list files in the current directory
 * > list
 */
void list_dir();

/**
 * softlink_setsrc(): set the source file of softlink
 * > softlink_setsrc <file_name>
 */
int softlink_setsrc(char *fn);

/**
 * softlink_setdst(): set the destination file of softlink
 * > softlink_setdst <file_name>
 */
int softlink_setdst(MyUser *mu, char *fn);

/**
 * hardlink_setsrc(): set the source file of hardlink
 * > hardlink_setsrc <file_name>
 */
int hardlink_setsrc(char *fn);

/**
 * hardlink_setdst(): set the destination file of hardlink
 * > hardlink_setdst <file_name>
 */
int hardlink_setdst(MyUser *mu, char *fn);

#endif