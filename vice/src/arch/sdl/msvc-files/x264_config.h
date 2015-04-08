#define X264_BIT_DEPTH     8
#define X264_GPL           1
#define X264_INTERLACED    1
#define X264_CHROMA_FORMAT 0
#define X264_VERSION ""
#define X264_POINTVER "0.142.x"

/* Disable some MSVC 7.x warnings for quicker compile */
#if (_MSC_VER == 1300) || (_MSC_VER == 1310) || (_MSC_VER == 1400)
#pragma warning(disable: 4003) /* not enough actual parameters for macro ... */
#pragma warning(disable: 4018) /* signed/unsigned mismatch */
#pragma warning(disable: 4090) /* different 'const' qualifiers */
#pragma warning(disable: 4244) /* conversion from ... to ..., possible loss of data */
#pragma warning(disable: 4305) /* truncation from 'double' to 'float' */
#pragma warning(disable: 4700) /* local variable ... used without having been initialized */
#endif
