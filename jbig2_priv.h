void *
jbig2_alloc (Jbig2Allocator *allocator, size_t size);

void
jbig2_free (Jbig2Allocator *allocator, void *p);

void *
jbig2_realloc (Jbig2Allocator *allocator, void *p, size_t size);

#define jbig2_new(ctx, t, size) ((t *)jbig2_alloc(ctx->allocator, (size) * sizeof(t)))

#define jbig2_renew(ctx, p, t, size) ((t *)jbig2_realloc(ctx->allocator, (p), (size) * sizeof(t)))

int
jbig2_error (Jbig2Ctx *ctx, Jbig2Severity severity, int32_t seg_idx,
	     const char *fmt, ...);

typedef uint8_t byte;
typedef int bool;

#define TRUE 1
#define FALSE 0

typedef struct _Jbig2Result Jbig2Result;

/* The result of decoding a segment. See 0.1.5 */
struct _Jbig2Result {
  int32_t segment_number;
  int segment_type;
  void (*free)(const Jbig2Result *self, Jbig2Ctx *ctx);
};

typedef enum {
  JBIG2_FILE_HEADER,
  JBIG2_FILE_SEQUENTIAL_HEADER,
  JBIG2_FILE_SEQUENTIAL_BODY,
  JBIG2_FILE_RANDOM_HEADERS,
  JBIG2_FILE_RANDOM_BODIES,
  JBIG2_FILE_EOF
} Jbig2FileState;

struct _Jbig2Ctx {
  Jbig2Allocator *allocator;
  Jbig2Options options;
  const Jbig2Ctx *global_ctx;
  Jbig2ErrorCallback error_callback;
  void *error_callback_data;

  byte *buf;
  int buf_size;
  int buf_rd_ix;
  int buf_wr_ix;

  Jbig2FileState state;

  byte file_header_flags;
  int32_t n_pages;

  int n_sh;
  int n_sh_max;
  Jbig2SegmentHeader **sh_list;
  int sh_ix;

  /* The map from segment numbers to decoding results, currently
     stored as a contiguous, 0-indexed array. */
  int n_results;
  int n_results_max;
  const Jbig2Result **results;
};

int32_t
jbig2_get_int32 (const byte *buf);

int16_t
jbig2_get_int16 (const byte *buf);

/* The word stream design is a compromise between simplicity and
   trying to amortize the number of method calls. Each ::get_next_word
   invocation pulls 4 bytes from the stream, packed big-endian into a
   32 bit word. The offset argument is provided as a convenience. It
   begins at 0 and increments by 4 for each successive invocation. */
typedef struct _Jbig2WordStream Jbig2WordStream;

struct _Jbig2WordStream {
  uint32_t (*get_next_word) (Jbig2WordStream *self, int offset);
};

Jbig2WordStream *
jbig2_word_stream_buf_new(Jbig2Ctx *ctx, const byte *data, size_t size);

void
jbig2_word_stream_buf_free(Jbig2Ctx *ctx, Jbig2WordStream *ws);

const Jbig2Result *
jbig2_get_result(Jbig2Ctx *ctx, int32_t segment_number);

int
jbig2_put_result(Jbig2Ctx *ctx, const Jbig2Result *result);

