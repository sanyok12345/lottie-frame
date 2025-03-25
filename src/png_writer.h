#ifndef PNG_WRITER_H
#define PNG_WRITER_H

#include "converter.h"
#include <png.h>

bool write_png(ConverterData* data, uint32_t* buffer);

#endif // PNG_WRITER_H