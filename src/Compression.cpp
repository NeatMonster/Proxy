#include "Compression.h"

#include "zlib/zlib.h"

#include <cstdlib>
#include <cstring>

size_t Compression::deflateZLib(ubyte_t *in, size_t inSize, ubyte_t *out, size_t outSize) {
    z_stream stream;
    std::memset(&stream, 0, sizeof(stream));
    stream.next_in = in;
    stream.avail_in = inSize;
    stream.next_out = out;
    stream.avail_out = outSize;
    deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    int ret = deflate(&stream, Z_FINISH);
    if (ret != Z_STREAM_END)
        throw CompressionException("deflate a retourné " + std::to_string(ret));
    outSize = stream.total_out;
    deflateEnd(&stream);
    return outSize;
}

size_t Compression::inflateZlib(ubyte_t *in, size_t inSize, ubyte_t *out, size_t outSize) {
    z_stream stream;
    stream.zalloc = (alloc_func) nullptr;
    stream.zfree = (free_func) nullptr;
    stream.opaque = nullptr;
    inflateInit(&stream);
    stream.next_in = in;
    stream.avail_in = inSize;
    stream.next_out = out;
    stream.avail_out = outSize;
    int ret = inflate(&stream, Z_NO_FLUSH);
    if (ret != Z_STREAM_END)
        throw new CompressionException("inflate a retourné " + std::to_string(ret));
    outSize = stream.total_out;
    inflateEnd(&stream);
    return outSize;
}
