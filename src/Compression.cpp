#include "Compression.h"

#include "zlib/zlib.h"

#include <cstdlib>
#include <cstring>

Compression::result_t Compression::deflateZLib(ubyte_t *in, size_t inSize) {
    size_t outSize;
    ubyte_t *out = new ubyte_t[inSize];
    z_stream stream;
    std::memset(&stream, 0, sizeof(stream));
    stream.next_in = in;
    stream.avail_in = inSize;
    stream.next_out = out;
    stream.avail_out = inSize;
    deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    int ret = deflate(&stream, Z_FINISH);
    if (ret == Z_STREAM_END)
        outSize = stream.total_out;
    else
        throw CompressionException("deflate a retourné " + std::to_string(ret));
    deflateEnd(&stream);
    return {out, outSize};
}

Compression::result_t Compression::inflateZlib(ubyte_t *in, size_t inSize) {
    size_t outSize = 0;
    std::vector<size_t> bufferSizes;
    std::vector<ubyte_t*> bufferPtrs;
    ubyte_t *buffer = new ubyte_t[BUFFER_SIZE];
    z_stream stream;
    stream.zalloc = (alloc_func) nullptr;
    stream.zfree = (free_func) nullptr;
    stream.opaque = nullptr;
    inflateInit(&stream);
    stream.next_in = in;
    stream.avail_in = inSize;
    stream.next_out = buffer;
    stream.avail_out = BUFFER_SIZE;
    while (true) {
        int ret = inflate(&stream, Z_NO_FLUSH);
        outSize += stream.total_out;
        bufferPtrs.push_back(buffer);
        bufferSizes.push_back(stream.total_out);
        if (ret == Z_OK) {
            buffer = new ubyte_t[BUFFER_SIZE];
            stream.next_out = buffer;
            stream.avail_out = BUFFER_SIZE;
        } else if (ret == Z_STREAM_END)
            break;
        else
            throw new CompressionException("Inflate a retourné " + std::to_string(ret));
    }
    inflateEnd(&stream);
    size_t currentSize = 0;
    ubyte_t *out = (ubyte_t*) std::malloc(outSize);
    for (size_t i = 0; i < bufferPtrs.size(); i++) {
        std::memcpy(out + currentSize, bufferPtrs[i], bufferSizes[i]);
        currentSize += bufferSizes[i];
        delete bufferPtrs[i];
    }
    return {out, outSize};
}
