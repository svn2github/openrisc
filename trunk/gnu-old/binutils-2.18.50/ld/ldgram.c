/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     NAME = 259,
     LNAME = 260,
     OREQ = 261,
     ANDEQ = 262,
     RSHIFTEQ = 263,
     LSHIFTEQ = 264,
     DIVEQ = 265,
     MULTEQ = 266,
     MINUSEQ = 267,
     PLUSEQ = 268,
     OROR = 269,
     ANDAND = 270,
     NE = 271,
     EQ = 272,
     GE = 273,
     LE = 274,
     RSHIFT = 275,
     LSHIFT = 276,
     UNARY = 277,
     END = 278,
     ALIGN_K = 279,
     BLOCK = 280,
     BIND = 281,
     QUAD = 282,
     SQUAD = 283,
     LONG = 284,
     SHORT = 285,
     BYTE = 286,
     SECTIONS = 287,
     PHDRS = 288,
     INSERT_K = 289,
     AFTER = 290,
     BEFORE = 291,
     DATA_SEGMENT_ALIGN = 292,
     DATA_SEGMENT_RELRO_END = 293,
     DATA_SEGMENT_END = 294,
     SORT_BY_NAME = 295,
     SORT_BY_ALIGNMENT = 296,
     SIZEOF_HEADERS = 297,
     OUTPUT_FORMAT = 298,
     FORCE_COMMON_ALLOCATION = 299,
     OUTPUT_ARCH = 300,
     INHIBIT_COMMON_ALLOCATION = 301,
     SEGMENT_START = 302,
     INCLUDE = 303,
     MEMORY = 304,
     NOLOAD = 305,
     DSECT = 306,
     COPY = 307,
     INFO = 308,
     OVERLAY = 309,
     DEFINED = 310,
     TARGET_K = 311,
     SEARCH_DIR = 312,
     MAP = 313,
     ENTRY = 314,
     NEXT = 315,
     SIZEOF = 316,
     ALIGNOF = 317,
     ADDR = 318,
     LOADADDR = 319,
     MAX_K = 320,
     MIN_K = 321,
     STARTUP = 322,
     HLL = 323,
     SYSLIB = 324,
     FLOAT = 325,
     NOFLOAT = 326,
     NOCROSSREFS = 327,
     ORIGIN = 328,
     FILL = 329,
     LENGTH = 330,
     CREATE_OBJECT_SYMBOLS = 331,
     INPUT = 332,
     GROUP = 333,
     OUTPUT = 334,
     CONSTRUCTORS = 335,
     ALIGNMOD = 336,
     AT = 337,
     SUBALIGN = 338,
     PROVIDE = 339,
     PROVIDE_HIDDEN = 340,
     AS_NEEDED = 341,
     CHIP = 342,
     LIST = 343,
     SECT = 344,
     ABSOLUTE = 345,
     LOAD = 346,
     NEWLINE = 347,
     ENDWORD = 348,
     ORDER = 349,
     NAMEWORD = 350,
     ASSERT_K = 351,
     FORMAT = 352,
     PUBLIC = 353,
     DEFSYMEND = 354,
     BASE = 355,
     ALIAS = 356,
     TRUNCATE = 357,
     REL = 358,
     INPUT_SCRIPT = 359,
     INPUT_MRI_SCRIPT = 360,
     INPUT_DEFSYM = 361,
     CASE = 362,
     EXTERN = 363,
     START = 364,
     VERS_TAG = 365,
     VERS_IDENTIFIER = 366,
     GLOBAL = 367,
     LOCAL = 368,
     VERSIONK = 369,
     INPUT_VERSION_SCRIPT = 370,
     KEEP = 371,
     ONLY_IF_RO = 372,
     ONLY_IF_RW = 373,
     SPECIAL = 374,
     EXCLUDE_FILE = 375,
     CONSTANT = 376,
     INPUT_DYNAMIC_LIST = 377
   };
#endif
#define INT 258
#define NAME 259
#define LNAME 260
#define OREQ 261
#define ANDEQ 262
#define RSHIFTEQ 263
#define LSHIFTEQ 264
#define DIVEQ 265
#define MULTEQ 266
#define MINUSEQ 267
#define PLUSEQ 268
#define OROR 269
#define ANDAND 270
#define NE 271
#define EQ 272
#define GE 273
#define LE 274
#define RSHIFT 275
#define LSHIFT 276
#define UNARY 277
#define END 278
#define ALIGN_K 279
#define BLOCK 280
#define BIND 281
#define QUAD 282
#define SQUAD 283
#define LONG 284
#define SHORT 285
#define BYTE 286
#define SECTIONS 287
#define PHDRS 288
#define INSERT_K 289
#define AFTER 290
#define BEFORE 291
#define DATA_SEGMENT_ALIGN 292
#define DATA_SEGMENT_RELRO_END 293
#define DATA_SEGMENT_END 294
#define SORT_BY_NAME 295
#define SORT_BY_ALIGNMENT 296
#define SIZEOF_HEADERS 297
#define OUTPUT_FORMAT 298
#define FORCE_COMMON_ALLOCATION 299
#define OUTPUT_ARCH 300
#define INHIBIT_COMMON_ALLOCATION 301
#define SEGMENT_START 302
#define INCLUDE 303
#define MEMORY 304
#define NOLOAD 305
#define DSECT 306
#define COPY 307
#define INFO 308
#define OVERLAY 309
#define DEFINED 310
#define TARGET_K 311
#define SEARCH_DIR 312
#define MAP 313
#define ENTRY 314
#define NEXT 315
#define SIZEOF 316
#define ALIGNOF 317
#define ADDR 318
#define LOADADDR 319
#define MAX_K 320
#define MIN_K 321
#define STARTUP 322
#define HLL 323
#define SYSLIB 324
#define FLOAT 325
#define NOFLOAT 326
#define NOCROSSREFS 327
#define ORIGIN 328
#define FILL 329
#define LENGTH 330
#define CREATE_OBJECT_SYMBOLS 331
#define INPUT 332
#define GROUP 333
#define OUTPUT 334
#define CONSTRUCTORS 335
#define ALIGNMOD 336
#define AT 337
#define SUBALIGN 338
#define PROVIDE 339
#define PROVIDE_HIDDEN 340
#define AS_NEEDED 341
#define CHIP 342
#define LIST 343
#define SECT 344
#define ABSOLUTE 345
#define LOAD 346
#define NEWLINE 347
#define ENDWORD 348
#define ORDER 349
#define NAMEWORD 350
#define ASSERT_K 351
#define FORMAT 352
#define PUBLIC 353
#define DEFSYMEND 354
#define BASE 355
#define ALIAS 356
#define TRUNCATE 357
#define REL 358
#define INPUT_SCRIPT 359
#define INPUT_MRI_SCRIPT 360
#define INPUT_DEFSYM 361
#define CASE 362
#define EXTERN 363
#define START 364
#define VERS_TAG 365
#define VERS_IDENTIFIER 366
#define GLOBAL 367
#define LOCAL 368
#define VERSIONK 369
#define INPUT_VERSION_SCRIPT 370
#define KEEP 371
#define ONLY_IF_RO 372
#define ONLY_IF_RW 373
#define SPECIAL 374
#define EXCLUDE_FILE 375
#define CONSTANT 376
#define INPUT_DYNAMIC_LIST 377




/* Copy the first part of user declarations.  */
#line 23 "ldgram.y"

/*

 */

#define DONTDECLARE_MALLOC

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "ld.h"
#include "ldexp.h"
#include "ldver.h"
#include "ldlang.h"
#include "ldfile.h"
#include "ldemul.h"
#include "ldmisc.h"
#include "ldmain.h"
#include "mri.h"
#include "ldctor.h"
#include "ldlex.h"

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

static enum section_type sectype;
static lang_memory_region_type *region;

bfd_boolean ldgram_had_keep = FALSE;
char *ldgram_vers_current_lang = NULL;

#define ERROR_NAME_MAX 20
static char *error_names[ERROR_NAME_MAX];
static int error_index;
#define PUSH_ERROR(x) if (error_index < ERROR_NAME_MAX) error_names[error_index] = x; error_index++;
#define POP_ERROR()   error_index--;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 61 "ldgram.y"
typedef union YYSTYPE {
  bfd_vma integer;
  struct big_int
    {
      bfd_vma integer;
      char *str;
    } bigint;
  fill_type *fill;
  char *name;
  const char *cname;
  struct wildcard_spec wildcard;
  struct wildcard_list *wildcard_list;
  struct name_list *name_list;
  int token;
  union etree_union *etree;
  struct phdr_info
    {
      bfd_boolean filehdr;
      bfd_boolean phdrs;
      union etree_union *at;
      union etree_union *flags;
    } phdr;
  struct lang_nocrossref *nocrossref;
  struct lang_output_section_phdr_list *section_phdr;
  struct bfd_elf_version_deps *deflist;
  struct bfd_elf_version_expr *versyms;
  struct bfd_elf_version_tree *versnode;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 388 "ldgram.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 400 "ldgram.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1800

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  146
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  122
/* YYNRULES -- Number of rules. */
#define YYNRULES  348
/* YYNRULES -- Number of states. */
#define YYNSTATES  742

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   377

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   144,     2,     2,     2,    34,    21,     2,
      37,   141,    32,    30,   139,    31,     2,    33,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    16,   140,
      24,     6,    25,    15,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   142,     2,   143,    20,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,    19,    57,   145,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     7,     8,     9,    10,    11,    12,    13,    14,    17,
      18,    22,    23,    26,    27,    28,    29,    35,    36,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    20,    21,
      26,    27,    30,    34,    35,    38,    43,    45,    47,    50,
      52,    57,    62,    66,    69,    74,    78,    83,    88,    93,
      98,   103,   106,   109,   112,   117,   122,   125,   128,   131,
     134,   135,   141,   144,   145,   149,   152,   153,   155,   159,
     161,   165,   166,   168,   172,   173,   176,   178,   181,   185,
     186,   189,   192,   193,   195,   197,   199,   201,   203,   205,
     207,   209,   211,   213,   218,   223,   228,   233,   242,   247,
     249,   251,   256,   257,   263,   268,   269,   275,   280,   285,
     289,   293,   295,   299,   302,   304,   308,   311,   312,   318,
     319,   327,   328,   335,   340,   343,   346,   347,   352,   355,
     356,   364,   366,   368,   370,   372,   378,   383,   388,   396,
     404,   412,   420,   429,   432,   434,   438,   440,   442,   446,
     451,   453,   454,   460,   463,   465,   467,   469,   474,   476,
     481,   486,   487,   496,   499,   501,   502,   504,   506,   508,
     510,   512,   514,   516,   519,   520,   522,   524,   526,   528,
     530,   532,   534,   536,   538,   540,   544,   548,   555,   562,
     564,   565,   571,   574,   578,   579,   580,   588,   592,   596,
     597,   601,   603,   606,   608,   611,   616,   621,   625,   629,
     631,   636,   640,   641,   643,   645,   646,   649,   653,   654,
     657,   660,   664,   669,   672,   675,   678,   682,   686,   690,
     694,   698,   702,   706,   710,   714,   718,   722,   726,   730,
     734,   738,   742,   748,   752,   756,   761,   763,   765,   770,
     775,   780,   785,   790,   795,   800,   807,   814,   821,   826,
     833,   838,   840,   847,   854,   861,   866,   871,   875,   876,
     881,   882,   887,   888,   893,   894,   896,   898,   900,   901,
     902,   903,   904,   905,   906,   926,   927,   928,   929,   930,
     931,   950,   951,   952,   960,   962,   964,   966,   968,   970,
     974,   975,   978,   982,   985,   992,  1003,  1006,  1008,  1009,
    1011,  1014,  1015,  1016,  1020,  1021,  1022,  1023,  1024,  1036,
    1041,  1042,  1045,  1046,  1047,  1054,  1056,  1057,  1061,  1067,
    1068,  1072,  1073,  1076,  1078,  1081,  1086,  1089,  1090,  1093,
    1094,  1100,  1102,  1105,  1110,  1116,  1123,  1125,  1128,  1129,
    1132,  1137,  1142,  1151,  1153,  1155,  1159,  1163,  1164,  1174,
    1175,  1183,  1185,  1189,  1191,  1195,  1197,  1201,  1202
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     147,     0,    -1,   120,   163,    -1,   121,   151,    -1,   131,
     256,    -1,   138,   251,    -1,   122,   149,    -1,     4,    -1,
      -1,   150,     4,     6,   213,    -1,    -1,   152,   153,    -1,
     153,   154,   108,    -1,    -1,   103,   213,    -1,   103,   213,
     139,   213,    -1,     4,    -1,   104,    -1,   110,   156,    -1,
     109,    -1,   114,     4,     6,   213,    -1,   114,     4,   139,
     213,    -1,   114,     4,   213,    -1,   113,     4,    -1,   105,
       4,   139,   213,    -1,   105,     4,   213,    -1,   105,     4,
       6,   213,    -1,    38,     4,     6,   213,    -1,    38,     4,
     139,   213,    -1,    97,     4,     6,   213,    -1,    97,     4,
     139,   213,    -1,   106,   158,    -1,   107,   157,    -1,   111,
       4,    -1,   117,     4,   139,     4,    -1,   117,     4,   139,
       3,    -1,   116,   213,    -1,   118,     3,    -1,   123,   159,
      -1,   124,   160,    -1,    -1,    64,   148,   155,   153,    36,
      -1,   125,     4,    -1,    -1,   156,   139,     4,    -1,   156,
       4,    -1,    -1,     4,    -1,   157,   139,     4,    -1,     4,
      -1,   158,   139,     4,    -1,    -1,     4,    -1,   159,   139,
       4,    -1,    -1,   161,   162,    -1,     4,    -1,   162,     4,
      -1,   162,   139,     4,    -1,    -1,   164,   165,    -1,   165,
     166,    -1,    -1,   195,    -1,   173,    -1,   243,    -1,   204,
      -1,   205,    -1,   207,    -1,   209,    -1,   175,    -1,   258,
      -1,   140,    -1,    72,    37,     4,   141,    -1,    73,    37,
     148,   141,    -1,    95,    37,   148,   141,    -1,    59,    37,
       4,   141,    -1,    59,    37,     4,   139,     4,   139,     4,
     141,    -1,    61,    37,     4,   141,    -1,    60,    -1,    62,
      -1,    93,    37,   169,   141,    -1,    -1,    94,   167,    37,
     169,   141,    -1,    74,    37,   148,   141,    -1,    -1,    64,
     148,   168,   165,    36,    -1,    88,    37,   210,   141,    -1,
     124,    37,   160,   141,    -1,    48,    49,     4,    -1,    48,
      50,     4,    -1,     4,    -1,   169,   139,     4,    -1,   169,
       4,    -1,     5,    -1,   169,   139,     5,    -1,   169,     5,
      -1,    -1,   102,    37,   170,   169,   141,    -1,    -1,   169,
     139,   102,    37,   171,   169,   141,    -1,    -1,   169,   102,
      37,   172,   169,   141,    -1,    46,    56,   174,    57,    -1,
     174,   219,    -1,   174,   175,    -1,    -1,    75,    37,     4,
     141,    -1,   193,   192,    -1,    -1,   112,   176,    37,   213,
     139,     4,   141,    -1,     4,    -1,    32,    -1,    15,    -1,
     177,    -1,   136,    37,   179,   141,   177,    -1,    54,    37,
     177,   141,    -1,    55,    37,   177,   141,    -1,    54,    37,
      55,    37,   177,   141,   141,    -1,    54,    37,    54,    37,
     177,   141,   141,    -1,    55,    37,    54,    37,   177,   141,
     141,    -1,    55,    37,    55,    37,   177,   141,   141,    -1,
      54,    37,   136,    37,   179,   141,   177,   141,    -1,   179,
     177,    -1,   177,    -1,   180,   194,   178,    -1,   178,    -1,
       4,    -1,   142,   180,   143,    -1,   178,    37,   180,   141,
      -1,   181,    -1,    -1,   132,    37,   183,   181,   141,    -1,
     193,   192,    -1,    92,    -1,   140,    -1,    96,    -1,    54,
      37,    96,   141,    -1,   182,    -1,   188,    37,   211,   141,
      -1,    90,    37,   189,   141,    -1,    -1,   112,   185,    37,
     213,   139,     4,   141,   192,    -1,   186,   184,    -1,   184,
      -1,    -1,   186,    -1,    41,    -1,    42,    -1,    43,    -1,
      44,    -1,    45,    -1,   211,    -1,     6,   189,    -1,    -1,
      14,    -1,    13,    -1,    12,    -1,    11,    -1,    10,    -1,
       9,    -1,     8,    -1,     7,    -1,   140,    -1,   139,    -1,
       4,     6,   211,    -1,     4,   191,   211,    -1,   100,    37,
       4,     6,   211,   141,    -1,   101,    37,     4,     6,   211,
     141,    -1,   139,    -1,    -1,    65,    56,   197,   196,    57,
      -1,   196,   197,    -1,   196,   139,   197,    -1,    -1,    -1,
       4,   198,   201,    16,   199,   194,   200,    -1,    89,     6,
     211,    -1,    91,     6,   211,    -1,    -1,    37,   202,   141,
      -1,   203,    -1,   202,   203,    -1,     4,    -1,   144,     4,
      -1,    83,    37,   148,   141,    -1,    84,    37,   206,   141,
      -1,    84,    37,   141,    -1,   206,   194,   148,    -1,   148,
      -1,    85,    37,   208,   141,    -1,   208,   194,   148,    -1,
      -1,    86,    -1,    87,    -1,    -1,     4,   210,    -1,     4,
     139,   210,    -1,    -1,   212,   213,    -1,    31,   213,    -1,
      37,   213,   141,    -1,    76,    37,   213,   141,    -1,   144,
     213,    -1,    30,   213,    -1,   145,   213,    -1,   213,    32,
     213,    -1,   213,    33,   213,    -1,   213,    34,   213,    -1,
     213,    30,   213,    -1,   213,    31,   213,    -1,   213,    29,
     213,    -1,   213,    28,   213,    -1,   213,    23,   213,    -1,
     213,    22,   213,    -1,   213,    27,   213,    -1,   213,    26,
     213,    -1,   213,    24,   213,    -1,   213,    25,   213,    -1,
     213,    21,   213,    -1,   213,    20,   213,    -1,   213,    19,
     213,    -1,   213,    15,   213,    16,   213,    -1,   213,    18,
     213,    -1,   213,    17,   213,    -1,    71,    37,     4,   141,
      -1,     3,    -1,    58,    -1,    78,    37,     4,   141,    -1,
      77,    37,     4,   141,    -1,    79,    37,     4,   141,    -1,
      80,    37,     4,   141,    -1,   137,    37,     4,   141,    -1,
     106,    37,   213,   141,    -1,    38,    37,   213,   141,    -1,
      38,    37,   213,   139,   213,   141,    -1,    51,    37,   213,
     139,   213,   141,    -1,    52,    37,   213,   139,   213,   141,
      -1,    53,    37,   213,   141,    -1,    63,    37,     4,   139,
     213,   141,    -1,    39,    37,   213,   141,    -1,     4,    -1,
      81,    37,   213,   139,   213,   141,    -1,    82,    37,   213,
     139,   213,   141,    -1,   112,    37,   213,   139,     4,   141,
      -1,    89,    37,     4,   141,    -1,    91,    37,     4,   141,
      -1,    98,    25,     4,    -1,    -1,    98,    37,   213,   141,
      -1,    -1,    38,    37,   213,   141,    -1,    -1,    99,    37,
     213,   141,    -1,    -1,   133,    -1,   134,    -1,   135,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     4,   220,   234,   215,
     216,   217,   221,   218,    56,   222,   187,    57,   223,   237,
     214,   238,   190,   224,   194,    -1,    -1,    -1,    -1,    -1,
      -1,    70,   225,   235,   236,   215,   217,   226,    56,   227,
     239,    57,   228,   237,   214,   238,   190,   229,   194,    -1,
      -1,    -1,    94,   230,   234,   231,    56,   174,    57,    -1,
      66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,    -1,
      37,   232,   141,    -1,    -1,    37,   141,    -1,   213,   233,
      16,    -1,   233,    16,    -1,    40,    37,   213,   141,   233,
      16,    -1,    40,    37,   213,   141,    39,    37,   213,   141,
     233,    16,    -1,   213,    16,    -1,    16,    -1,    -1,    88,
      -1,    25,     4,    -1,    -1,    -1,   238,    16,     4,    -1,
      -1,    -1,    -1,    -1,   239,     4,   240,    56,   187,    57,
     241,   238,   190,   242,   194,    -1,    47,    56,   244,    57,
      -1,    -1,   244,   245,    -1,    -1,    -1,     4,   246,   248,
     249,   247,   140,    -1,   213,    -1,    -1,     4,   250,   249,
      -1,    98,    37,   213,   141,   249,    -1,    -1,    37,   213,
     141,    -1,    -1,   252,   253,    -1,   254,    -1,   253,   254,
      -1,    56,   255,    57,   140,    -1,   264,   140,    -1,    -1,
     257,   260,    -1,    -1,   259,   130,    56,   260,    57,    -1,
     261,    -1,   260,   261,    -1,    56,   263,    57,   140,    -1,
     126,    56,   263,    57,   140,    -1,   126,    56,   263,    57,
     262,   140,    -1,   126,    -1,   262,   126,    -1,    -1,   264,
     140,    -1,   128,    16,   264,   140,    -1,   129,    16,   264,
     140,    -1,   128,    16,   264,   140,   129,    16,   264,   140,
      -1,   127,    -1,     4,    -1,   264,   140,   127,    -1,   264,
     140,     4,    -1,    -1,   264,   140,   124,     4,    56,   265,
     264,   267,    57,    -1,    -1,   124,     4,    56,   266,   264,
     267,    57,    -1,   128,    -1,   264,   140,   128,    -1,   129,
      -1,   264,   140,   129,    -1,   124,    -1,   264,   140,   124,
      -1,    -1,   140,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   160,   160,   161,   162,   163,   164,   168,   172,   172,
     182,   182,   195,   196,   200,   201,   202,   205,   208,   209,
     210,   212,   214,   216,   218,   220,   222,   224,   226,   228,
     230,   232,   233,   234,   236,   238,   240,   242,   244,   245,
     247,   246,   250,   252,   256,   257,   258,   262,   264,   268,
     270,   275,   276,   277,   282,   282,   287,   289,   291,   296,
     296,   302,   303,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   320,   322,   324,   327,   329,   331,
     333,   335,   337,   336,   340,   343,   342,   346,   350,   351,
     353,   358,   361,   364,   367,   370,   373,   377,   376,   381,
     380,   385,   384,   391,   395,   396,   397,   401,   403,   404,
     404,   412,   416,   420,   427,   433,   439,   445,   451,   457,
     463,   469,   475,   484,   493,   504,   513,   524,   532,   536,
     543,   545,   544,   551,   552,   556,   557,   562,   567,   568,
     573,   577,   577,   583,   584,   587,   589,   593,   595,   597,
     599,   601,   606,   613,   615,   619,   621,   623,   625,   627,
     629,   631,   633,   638,   638,   643,   647,   655,   659,   667,
     667,   671,   675,   676,   677,   682,   681,   689,   697,   705,
     706,   710,   711,   715,   717,   722,   727,   728,   733,   735,
     741,   743,   745,   749,   751,   757,   760,   769,   780,   780,
     786,   788,   790,   792,   794,   796,   799,   801,   803,   805,
     807,   809,   811,   813,   815,   817,   819,   821,   823,   825,
     827,   829,   831,   833,   835,   837,   839,   841,   844,   846,
     848,   850,   852,   854,   856,   858,   860,   862,   864,   866,
     875,   877,   879,   881,   883,   885,   887,   893,   894,   898,
     899,   903,   904,   908,   909,   913,   914,   915,   916,   919,
     923,   926,   932,   934,   919,   941,   943,   945,   950,   952,
     940,   962,   964,   962,   972,   973,   974,   975,   976,   980,
     981,   982,   986,   987,   992,   993,   998,   999,  1004,  1005,
    1010,  1012,  1017,  1020,  1033,  1037,  1042,  1044,  1035,  1052,
    1055,  1057,  1061,  1062,  1061,  1071,  1116,  1119,  1131,  1140,
    1143,  1150,  1150,  1162,  1163,  1167,  1171,  1180,  1180,  1194,
    1194,  1204,  1205,  1209,  1213,  1217,  1224,  1228,  1236,  1239,
    1243,  1247,  1251,  1258,  1262,  1266,  1270,  1275,  1274,  1288,
    1287,  1297,  1301,  1305,  1309,  1313,  1317,  1323,  1325
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "NAME", "LNAME", "'='", "OREQ",
  "ANDEQ", "RSHIFTEQ", "LSHIFTEQ", "DIVEQ", "MULTEQ", "MINUSEQ", "PLUSEQ",
  "'?'", "':'", "OROR", "ANDAND", "'|'", "'^'", "'&'", "NE", "EQ", "'<'",
  "'>'", "GE", "LE", "RSHIFT", "LSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'",
  "UNARY", "END", "'('", "ALIGN_K", "BLOCK", "BIND", "QUAD", "SQUAD",
  "LONG", "SHORT", "BYTE", "SECTIONS", "PHDRS", "INSERT_K", "AFTER",
  "BEFORE", "DATA_SEGMENT_ALIGN", "DATA_SEGMENT_RELRO_END",
  "DATA_SEGMENT_END", "SORT_BY_NAME", "SORT_BY_ALIGNMENT", "'{'", "'}'",
  "SIZEOF_HEADERS", "OUTPUT_FORMAT", "FORCE_COMMON_ALLOCATION",
  "OUTPUT_ARCH", "INHIBIT_COMMON_ALLOCATION", "SEGMENT_START", "INCLUDE",
  "MEMORY", "NOLOAD", "DSECT", "COPY", "INFO", "OVERLAY", "DEFINED",
  "TARGET_K", "SEARCH_DIR", "MAP", "ENTRY", "NEXT", "SIZEOF", "ALIGNOF",
  "ADDR", "LOADADDR", "MAX_K", "MIN_K", "STARTUP", "HLL", "SYSLIB",
  "FLOAT", "NOFLOAT", "NOCROSSREFS", "ORIGIN", "FILL", "LENGTH",
  "CREATE_OBJECT_SYMBOLS", "INPUT", "GROUP", "OUTPUT", "CONSTRUCTORS",
  "ALIGNMOD", "AT", "SUBALIGN", "PROVIDE", "PROVIDE_HIDDEN", "AS_NEEDED",
  "CHIP", "LIST", "SECT", "ABSOLUTE", "LOAD", "NEWLINE", "ENDWORD",
  "ORDER", "NAMEWORD", "ASSERT_K", "FORMAT", "PUBLIC", "DEFSYMEND", "BASE",
  "ALIAS", "TRUNCATE", "REL", "INPUT_SCRIPT", "INPUT_MRI_SCRIPT",
  "INPUT_DEFSYM", "CASE", "EXTERN", "START", "VERS_TAG", "VERS_IDENTIFIER",
  "GLOBAL", "LOCAL", "VERSIONK", "INPUT_VERSION_SCRIPT", "KEEP",
  "ONLY_IF_RO", "ONLY_IF_RW", "SPECIAL", "EXCLUDE_FILE", "CONSTANT",
  "INPUT_DYNAMIC_LIST", "','", "';'", "')'", "'['", "']'", "'!'", "'~'",
  "$accept", "file", "filename", "defsym_expr", "@1", "mri_script_file",
  "@2", "mri_script_lines", "mri_script_command", "@3", "ordernamelist",
  "mri_load_name_list", "mri_abs_name_list", "casesymlist",
  "extern_name_list", "@4", "extern_name_list_body", "script_file", "@5",
  "ifile_list", "ifile_p1", "@6", "@7", "input_list", "@8", "@9", "@10",
  "sections", "sec_or_group_p1", "statement_anywhere", "@11",
  "wildcard_name", "wildcard_spec", "exclude_name_list", "file_NAME_list",
  "input_section_spec_no_keep", "input_section_spec", "@12", "statement",
  "@13", "statement_list", "statement_list_opt", "length", "fill_exp",
  "fill_opt", "assign_op", "end", "assignment", "opt_comma", "memory",
  "memory_spec_list", "memory_spec", "@14", "origin_spec", "length_spec",
  "attributes_opt", "attributes_list", "attributes_string", "startup",
  "high_level_library", "high_level_library_NAME_list",
  "low_level_library", "low_level_library_NAME_list",
  "floating_point_support", "nocrossref_list", "mustbe_exp", "@15", "exp",
  "memspec_at_opt", "opt_at", "opt_align", "opt_subalign",
  "sect_constraint", "section", "@16", "@17", "@18", "@19", "@20", "@21",
  "@22", "@23", "@24", "@25", "@26", "@27", "type", "atype",
  "opt_exp_with_type", "opt_exp_without_type", "opt_nocrossrefs",
  "memspec_opt", "phdr_opt", "overlay_section", "@28", "@29", "@30",
  "phdrs", "phdr_list", "phdr", "@31", "@32", "phdr_type",
  "phdr_qualifiers", "phdr_val", "dynamic_list_file", "@33",
  "dynamic_list_nodes", "dynamic_list_node", "dynamic_list_tag",
  "version_script_file", "@34", "version", "@35", "vers_nodes",
  "vers_node", "verdep", "vers_tag", "vers_defns", "@36", "@37",
  "opt_semicolon", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,    61,   261,   262,   263,
     264,   265,   266,   267,   268,    63,    58,   269,   270,   124,
      94,    38,   271,   272,    60,    62,   273,   274,   275,   276,
      43,    45,    42,    47,    37,   277,   278,    40,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   123,   125,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,   374,   375,   376,   377,    44,
      59,    41,    91,    93,    33,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   146,   147,   147,   147,   147,   147,   148,   150,   149,
     152,   151,   153,   153,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     155,   154,   154,   154,   156,   156,   156,   157,   157,   158,
     158,   159,   159,   159,   161,   160,   162,   162,   162,   164,
     163,   165,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   167,   166,   166,   168,   166,   166,   166,   166,
     166,   169,   169,   169,   169,   169,   169,   170,   169,   171,
     169,   172,   169,   173,   174,   174,   174,   175,   175,   176,
     175,   177,   177,   177,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   179,   179,   180,   180,   181,   181,   181,
     182,   183,   182,   184,   184,   184,   184,   184,   184,   184,
     184,   185,   184,   186,   186,   187,   187,   188,   188,   188,
     188,   188,   189,   190,   190,   191,   191,   191,   191,   191,
     191,   191,   191,   192,   192,   193,   193,   193,   193,   194,
     194,   195,   196,   196,   196,   198,   197,   199,   200,   201,
     201,   202,   202,   203,   203,   204,   205,   205,   206,   206,
     207,   208,   208,   209,   209,   210,   210,   210,   212,   211,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   214,   214,   215,
     215,   216,   216,   217,   217,   218,   218,   218,   218,   220,
     221,   222,   223,   224,   219,   225,   226,   227,   228,   229,
     219,   230,   231,   219,   232,   232,   232,   232,   232,   233,
     233,   233,   234,   234,   234,   234,   235,   235,   236,   236,
     237,   237,   238,   238,   239,   240,   241,   242,   239,   243,
     244,   244,   246,   247,   245,   248,   249,   249,   249,   250,
     250,   252,   251,   253,   253,   254,   255,   257,   256,   259,
     258,   260,   260,   261,   261,   261,   262,   262,   263,   263,
     263,   263,   263,   264,   264,   264,   264,   265,   264,   266,
     264,   264,   264,   264,   264,   264,   264,   267,   267
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     1,     0,     4,
       0,     2,     3,     0,     2,     4,     1,     1,     2,     1,
       4,     4,     3,     2,     4,     3,     4,     4,     4,     4,
       4,     2,     2,     2,     4,     4,     2,     2,     2,     2,
       0,     5,     2,     0,     3,     2,     0,     1,     3,     1,
       3,     0,     1,     3,     0,     2,     1,     2,     3,     0,
       2,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     4,     4,     4,     8,     4,     1,
       1,     4,     0,     5,     4,     0,     5,     4,     4,     3,
       3,     1,     3,     2,     1,     3,     2,     0,     5,     0,
       7,     0,     6,     4,     2,     2,     0,     4,     2,     0,
       7,     1,     1,     1,     1,     5,     4,     4,     7,     7,
       7,     7,     8,     2,     1,     3,     1,     1,     3,     4,
       1,     0,     5,     2,     1,     1,     1,     4,     1,     4,
       4,     0,     8,     2,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     6,     6,     1,
       0,     5,     2,     3,     0,     0,     7,     3,     3,     0,
       3,     1,     2,     1,     2,     4,     4,     3,     3,     1,
       4,     3,     0,     1,     1,     0,     2,     3,     0,     2,
       2,     3,     4,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     5,     3,     3,     4,     1,     1,     4,     4,
       4,     4,     4,     4,     4,     6,     6,     6,     4,     6,
       4,     1,     6,     6,     6,     4,     4,     3,     0,     4,
       0,     4,     0,     4,     0,     1,     1,     1,     0,     0,
       0,     0,     0,     0,    19,     0,     0,     0,     0,     0,
      18,     0,     0,     7,     1,     1,     1,     1,     1,     3,
       0,     2,     3,     2,     6,    10,     2,     1,     0,     1,
       2,     0,     0,     3,     0,     0,     0,     0,    11,     4,
       0,     2,     0,     0,     6,     1,     0,     3,     5,     0,
       3,     0,     2,     1,     2,     4,     2,     0,     2,     0,
       5,     1,     2,     4,     5,     6,     1,     2,     0,     2,
       4,     4,     8,     1,     1,     3,     3,     0,     9,     0,
       7,     1,     3,     1,     3,     1,     3,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       0,    59,    10,     8,   317,   311,     0,     2,    62,     3,
      13,     6,     0,     4,     0,     5,     0,     1,    60,    11,
       0,   328,     0,   318,   321,     0,   312,   313,     0,     0,
       0,     0,     0,    79,     0,    80,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   193,   194,     0,     0,    82,
       0,     0,     0,   109,     0,    72,    61,    64,    70,     0,
      63,    66,    67,    68,    69,    65,    71,     0,    16,     0,
       0,     0,     0,    17,     0,     0,     0,    19,    46,     0,
       0,     0,     0,     0,     0,    51,    54,     0,     0,     0,
     334,   345,   333,   341,   343,     0,     0,   328,   322,   341,
     343,     0,     0,   314,   198,   162,   161,   160,   159,   158,
     157,   156,   155,   198,   106,   300,     0,     0,     0,     0,
       7,    85,     0,     0,     0,     0,     0,     0,     0,   192,
     195,     0,     0,     0,     0,     0,     0,    54,   164,   163,
     108,     0,     0,    40,     0,   226,   241,     0,     0,     0,
       0,     0,     0,     0,     0,   227,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,    49,    31,    47,    32,    18,    33,
      23,     0,    36,     0,    37,    52,    38,    39,     0,    42,
      12,     9,     0,     0,     0,     0,   329,     0,     0,   316,
     165,     0,   166,     0,     0,    89,    90,     0,     0,    62,
     175,   174,     0,     0,     0,     0,     0,   187,   189,   170,
     170,   195,     0,    91,    94,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,   204,
     200,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   203,   205,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,     0,     0,    45,     0,
       0,     0,    22,     0,     0,    56,    55,   339,     0,     0,
     323,   336,   346,   335,   342,   344,     0,   315,   199,   259,
     103,   265,   271,   105,   104,   302,   299,   301,     0,    76,
      78,   319,   179,     0,    73,    74,    84,   107,   185,   169,
     186,     0,   190,     0,   195,   196,    87,    97,    93,    96,
       0,     0,    81,     0,    75,   198,   198,     0,    88,     0,
      27,    28,    43,    29,    30,   201,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   224,   223,   221,   220,
     219,   214,   213,   217,   218,   216,   215,   212,   211,   209,
     210,   206,   207,   208,    15,    26,    24,    50,    48,    44,
      20,    21,    35,    34,    53,    57,     0,     0,   330,   331,
       0,   326,   324,     0,   280,     0,   280,     0,     0,    86,
       0,     0,   171,     0,   172,   188,   191,   197,     0,   101,
      92,    95,     0,    83,     0,     0,     0,   320,    41,     0,
     234,   240,     0,     0,   238,     0,   225,   202,   229,   228,
     230,   231,     0,     0,   245,   246,   233,     0,   232,     0,
      58,   347,   344,   337,   327,   325,     0,     0,   280,     0,
     250,   287,     0,   288,   272,   305,   306,     0,   183,     0,
       0,   181,     0,   173,     0,     0,    99,   167,   168,     0,
       0,     0,     0,     0,     0,     0,     0,   222,   348,     0,
       0,     0,   274,   275,   276,   277,   278,   281,     0,     0,
       0,     0,   283,     0,   252,   286,   289,   250,     0,   309,
       0,   303,     0,   184,   180,   182,     0,   170,    98,     0,
       0,   110,   235,   236,   237,   239,   242,   243,   244,   340,
       0,   347,   279,     0,   282,     0,     0,   254,   254,   106,
       0,   306,     0,     0,    77,   198,     0,   102,     0,   332,
       0,   280,     0,     0,     0,   260,   266,     0,     0,   307,
       0,   304,   177,     0,   176,   100,   338,     0,     0,   249,
       0,     0,   258,     0,   273,   310,   306,   198,     0,   284,
     251,     0,   255,   256,   257,     0,   267,   308,   178,     0,
     253,   261,   294,   280,   145,     0,     0,   127,   113,   112,
     147,   148,   149,   150,   151,     0,     0,     0,   134,   136,
     141,     0,     0,   135,     0,   114,     0,   130,   138,   144,
     146,     0,     0,     0,   295,   268,   285,     0,     0,   198,
       0,   131,     0,   111,     0,   126,   170,     0,   143,   262,
     198,   133,     0,   291,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   152,     0,     0,   124,     0,     0,   128,
       0,   170,   291,     0,   145,     0,   248,     0,     0,   137,
       0,   116,     0,     0,   117,   140,     0,   111,     0,     0,
     123,   125,   129,   248,   139,     0,   290,     0,   292,     0,
       0,     0,     0,     0,     0,   132,   115,   292,   296,     0,
     154,     0,     0,     0,     0,     0,     0,   154,   292,   247,
     198,     0,   269,   119,   118,     0,   120,   121,     0,   263,
     154,   153,   293,   170,   122,   142,   170,   297,   270,   264,
     170,   298
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     6,   121,    11,    12,     9,    10,    19,    88,   236,
     178,   177,   175,   186,   187,   188,   296,     7,     8,    18,
      56,   132,   209,   226,   428,   530,   485,    57,   203,    58,
     136,   625,   626,   667,   646,   627,   628,   665,   629,   640,
     630,   631,   632,   662,   722,   113,   140,    59,   670,    60,
     323,   211,   322,   527,   574,   421,   480,   481,    61,    62,
     219,    63,   220,    64,   222,   663,   201,   241,   698,   514,
     547,   565,   595,   314,   414,   582,   604,   672,   736,   415,
     583,   602,   653,   733,   416,   518,   508,   469,   470,   473,
     517,   676,   710,   605,   652,   718,   740,    65,   204,   317,
     417,   553,   476,   521,   551,    15,    16,    26,    27,   101,
      13,    14,    66,    67,    23,    24,   413,    95,    96,   501,
     407,   499
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -606
static const short yypact[] =
{
     187,  -606,  -606,  -606,  -606,  -606,    41,  -606,  -606,  -606,
    -606,  -606,    32,  -606,     0,  -606,    24,  -606,   769,  1520,
      67,     9,    56,     0,  -606,   104,    24,  -606,   800,    69,
      83,     8,   117,  -606,   131,  -606,   143,   123,   192,   203,
     209,   220,   228,   234,   235,  -606,  -606,   239,   257,  -606,
     266,   276,   279,  -606,   291,  -606,  -606,  -606,  -606,    50,
    -606,  -606,  -606,  -606,  -606,  -606,  -606,    90,  -606,   325,
     143,   326,   714,  -606,   327,   330,   331,  -606,  -606,   332,
     335,   336,   714,   337,   343,   347,  -606,   349,   246,   714,
    -606,   362,  -606,   352,   353,   290,   236,     9,  -606,  -606,
    -606,   315,   238,  -606,  -606,  -606,  -606,  -606,  -606,  -606,
    -606,  -606,  -606,  -606,  -606,  -606,   370,   379,   380,   381,
    -606,  -606,   382,   384,   143,   143,   386,   143,    12,  -606,
     409,    19,   345,   143,   410,   411,   383,  -606,  -606,  -606,
    -606,   360,    21,  -606,    25,  -606,  -606,   714,   714,   714,
     385,   387,   391,   392,   393,  -606,   395,   396,   397,   398,
     399,   401,   405,   406,   407,   408,   412,   413,   415,   417,
     714,   714,  1340,   388,  -606,   278,  -606,   284,    77,  -606,
    -606,   484,  1718,   308,  -606,  -606,   309,  -606,   451,  -606,
    -606,  1718,   365,   104,   104,   316,   110,   400,   318,   110,
    -606,   714,  -606,   263,    35,  -606,  -606,   144,   319,  -606,
    -606,  -606,   320,   321,   322,   333,   334,  -606,  -606,   145,
     173,    78,   340,  -606,  -606,   434,    17,    19,   348,   466,
     470,   714,   351,     0,   714,   714,  -606,   714,   714,  -606,
    -606,   378,   714,   714,   714,   714,   714,   480,   482,   714,
     489,   491,   492,   493,   714,   714,   494,   495,   714,   714,
     497,  -606,  -606,   714,   714,   714,   714,   714,   714,   714,
     714,   714,   714,   714,   714,   714,   714,   714,   714,   714,
     714,   714,   714,   714,   714,  1718,   498,   500,  -606,   501,
     714,   714,  1718,   232,   502,  -606,    85,  -606,   367,   368,
    -606,  -606,   506,  -606,  -606,  -606,   -52,  -606,  1718,   800,
    -606,  -606,  -606,  -606,  -606,  -606,  -606,  -606,   507,  -606,
    -606,   841,   475,    22,  -606,  -606,  -606,  -606,  -606,  -606,
    -606,   143,  -606,   143,   409,  -606,  -606,  -606,  -606,  -606,
     476,    30,  -606,    28,  -606,  -606,  -606,  1360,  -606,    -8,
    1718,  1718,  1544,  1718,  1718,  -606,   965,   985,  1380,  1400,
    1005,   389,   377,  1025,   390,   402,   403,   404,  1420,  1440,
     416,   418,  1045,  1463,   429,  1678,  1735,  1751,  1766,   705,
     944,   730,   730,   628,   628,   628,   628,   328,   328,    71,
      71,  -606,  -606,  -606,  1718,  1718,  1718,  -606,  -606,  -606,
    1718,  1718,  -606,  -606,  -606,  -606,   522,   104,   177,   110,
     473,  -606,  -606,   -43,   564,   634,   564,   714,   414,  -606,
       5,   514,  -606,   382,  -606,  -606,  -606,  -606,    19,  -606,
    -606,  -606,   503,  -606,   430,   431,   530,  -606,  -606,   714,
    -606,  -606,   714,   714,  -606,   714,  -606,  -606,  -606,  -606,
    -606,  -606,   714,   714,  -606,  -606,  -606,   534,  -606,   714,
    -606,   445,   523,  -606,  -606,  -606,   211,   504,  1655,   533,
     448,  -606,  1698,   462,  -606,  1718,    33,   547,  -606,   548,
       4,  -606,   465,  -606,    39,    19,  -606,  -606,  -606,   446,
    1065,  1090,  1110,  1130,  1150,  1170,   447,  1718,   110,   529,
     104,   104,  -606,  -606,  -606,  -606,  -606,  -606,   450,   714,
     185,   542,  -606,   552,   559,  -606,  -606,   448,   549,   561,
     569,  -606,   467,  -606,  -606,  -606,   603,   471,  -606,    82,
      19,  -606,  -606,  -606,  -606,  -606,  -606,  -606,  -606,  -606,
     472,   445,  -606,  1190,  -606,   714,   576,   515,   515,  -606,
     714,    33,   714,   478,  -606,  -606,   528,  -606,    86,   110,
     563,   282,  1215,   714,   587,  -606,  -606,   499,  1235,  -606,
    1255,  -606,  -606,   619,  -606,  -606,  -606,   589,   614,  -606,
    1275,   714,   126,   575,  -606,  -606,    33,  -606,   714,  -606,
    -606,  1295,  -606,  -606,  -606,   577,  -606,  -606,  -606,  1315,
    -606,  -606,  -606,   595,    10,    36,   618,   570,  -606,  -606,
    -606,  -606,  -606,  -606,  -606,   599,   602,   610,  -606,  -606,
    -606,   611,   612,  -606,   243,  -606,   615,  -606,  -606,  -606,
      10,   594,   617,    50,  -606,  -606,  -606,   241,    62,  -606,
     626,  -606,    63,  -606,   629,  -606,    87,   243,  -606,  -606,
    -606,  -606,   613,   642,   631,   637,   536,   638,   537,   643,
     644,   538,   541,  -606,   714,    15,  -606,    14,   295,  -606,
     243,   183,   642,   543,    10,   679,   590,    63,    63,  -606,
      63,  -606,    63,    63,  -606,  -606,  1483,   550,   553,    63,
    -606,  -606,  -606,   590,  -606,   632,  -606,   665,  -606,   554,
     555,    81,   557,   558,   689,  -606,  -606,  -606,  -606,   696,
     128,   562,   565,    63,   566,   578,   579,   128,  -606,  -606,
    -606,   698,  -606,  -606,  -606,   580,  -606,  -606,    50,  -606,
     128,  -606,  -606,   471,  -606,  -606,   471,  -606,  -606,  -606,
     471,  -606
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -606,  -606,   -65,  -606,  -606,  -606,  -606,   468,  -606,  -606,
    -606,  -606,  -606,  -606,   585,  -606,  -606,  -606,  -606,   532,
    -606,  -606,  -606,  -215,  -606,  -606,  -606,  -606,   175,  -202,
    -606,   884,  -586,    68,    95,    84,  -606,  -606,   113,  -606,
    -606,    73,  -606,    48,  -590,  -606,  -605,  -559,  -216,  -606,
    -606,  -303,  -606,  -606,  -606,  -606,  -606,   270,  -606,  -606,
    -606,  -606,  -606,  -606,  -206,  -102,  -606,   -72,    76,   253,
    -606,   226,  -606,  -606,  -606,  -606,  -606,  -606,  -606,  -606,
    -606,  -606,  -606,  -606,  -606,  -606,  -606,  -418,   359,  -606,
    -606,   108,  -583,  -606,  -606,  -606,  -606,  -606,  -606,  -606,
    -606,  -606,  -606,  -479,  -606,  -606,  -606,  -606,   750,  -606,
    -606,  -606,  -606,  -606,   551,   -17,  -606,   684,   -18,  -606,
    -606,   242
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -320
static const short yytable[] =
{
     172,   313,   200,   331,   333,   143,    98,   102,   478,   478,
     182,   202,   343,    90,   607,   335,   120,   191,   643,   687,
     424,   338,   339,   223,   224,   608,   210,   234,   651,   608,
     608,   237,   338,   339,   430,   431,    20,   519,   645,   315,
     634,    17,   609,   338,   339,   633,   609,   609,    21,   437,
     511,   610,   611,   612,   613,   614,    21,   116,   117,   213,
     214,   645,   216,   218,   615,   616,   643,   643,   228,   644,
     616,   633,   569,    89,   411,   239,   240,   608,   608,   422,
      25,   288,   221,   464,   691,   643,   338,   339,   412,   405,
     338,   339,   316,   635,   609,   609,   608,   465,   261,   262,
     617,   285,   618,   279,   280,   281,   619,   597,    90,   292,
      51,    52,    97,   609,   301,   633,   659,   660,    22,   340,
     483,   225,   620,   735,   717,   114,    22,   729,   427,   308,
     340,   520,   432,    91,   720,   730,    92,    93,    94,   115,
     737,   340,   621,   578,   721,   524,   622,   120,   479,   479,
     623,   622,   624,   217,   118,   689,   341,   624,   342,   347,
     235,   423,   350,   351,   238,   353,   354,   341,   119,   433,
     356,   357,   358,   359,   360,   298,   299,   363,   341,   122,
     528,   301,   368,   369,   340,   606,   372,   373,   340,   138,
     139,   375,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   484,   145,   146,   289,   334,   400,   401,
     141,   341,   713,   557,   406,   341,   329,   575,    91,   123,
     669,    92,    99,   100,   302,   402,   403,   303,   304,   305,
     124,   147,   148,   434,   435,   643,   125,   643,   149,   150,
     151,   502,   503,   504,   505,   506,   608,   126,   608,   592,
     593,   594,   152,   153,   154,   127,   425,   309,   426,   155,
     529,   128,   129,   609,   156,   609,   130,   502,   503,   504,
     505,   506,   157,   318,   329,   319,   330,   158,   159,   160,
     161,   162,   163,   164,   131,   654,   655,   644,   616,   643,
     165,   302,   166,   133,   303,   304,   462,     1,     2,     3,
     608,   556,   329,   134,   332,   558,   135,   167,     4,   510,
     310,   577,   329,   168,   692,     5,   507,   609,   137,   142,
     144,   173,    98,   311,   174,   176,   179,   656,    41,   180,
     181,   183,   468,   472,   468,   475,   184,   195,   169,   654,
     655,   185,   507,   189,   190,   170,   171,   312,   277,   278,
     279,   280,   281,    51,    52,   313,   192,   490,   193,   194,
     491,   492,   198,   493,   205,    53,   196,   657,   199,   622,
     494,   495,   227,   206,   207,   208,   210,   497,   212,   461,
     215,   145,   146,   263,   283,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   221,   229,   230,   233,   286,   147,   148,
     231,   297,   242,   287,   243,   149,   150,   151,   244,   245,
     246,   657,   247,   248,   249,   250,   251,   543,   252,   152,
     153,   154,   253,   254,   255,   256,   155,   293,   294,   257,
     258,   156,   259,   572,   260,   295,   300,   306,   307,   157,
     320,   324,   325,   326,   158,   159,   160,   161,   162,   163,
     164,   337,   345,   562,   327,   328,   346,   165,   568,   166,
     570,   336,   540,   541,   361,   598,   362,   145,   146,   344,
     290,   580,   348,   364,   167,   365,   366,   367,   370,   371,
     168,   374,   397,   309,   398,   399,   404,   408,   409,   591,
     410,   418,   420,   429,   147,   148,   599,   738,   446,   355,
     739,   149,   150,   151,   741,   169,   460,   284,   445,   463,
     482,   448,   170,   171,   489,   152,   153,   154,   496,   500,
     486,   509,   155,   449,   450,   451,   513,   156,   673,   512,
     516,   522,   523,   477,   526,   157,   584,   454,   544,   455,
     158,   159,   160,   161,   162,   163,   164,   145,   146,   311,
     458,   487,   488,   165,    41,   166,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   498,   539,   531,   538,   545,
     167,   542,   686,   312,   147,   148,   168,   546,   550,    51,
      52,   466,   150,   151,   467,   549,   552,  -111,   554,   555,
     329,    53,   559,   563,   564,   152,   153,   154,   571,   573,
     576,   169,   155,   291,   581,   587,   588,   156,   170,   171,
     589,   596,   510,   601,   636,   157,   637,   145,   146,   638,
     158,   159,   160,   161,   162,   163,   164,   639,   641,   642,
     471,   649,   647,   165,   650,   166,   275,   276,   277,   278,
     279,   280,   281,   664,   147,   148,   668,   675,   677,   674,
     167,   149,   150,   151,   678,   680,   168,   679,   681,   684,
     682,   683,   685,   696,   694,   152,   153,   154,   697,   708,
     709,  -127,   155,   716,   705,   711,   712,   156,   714,   715,
     719,   169,   732,   723,   352,   157,   724,   726,   170,   171,
     158,   159,   160,   161,   162,   163,   164,   145,   146,   727,
     728,   734,   232,   165,   567,   166,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     167,   321,   671,   648,   147,   148,   168,   695,   701,   688,
     525,   149,   150,   151,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   152,   153,   154,   731,   707,
     548,   169,   155,    28,   566,   474,   103,   156,   170,   171,
     693,   197,     0,   560,   349,   157,     0,     0,     0,     0,
     158,   159,   160,   161,   162,   163,   164,     0,     0,     0,
       0,     0,     0,   165,     0,   166,   104,   105,   106,   107,
     108,   109,   110,   111,   112,    29,    30,    31,     0,     0,
     167,     0,     0,     0,     0,     0,   168,     0,    32,    33,
      34,    35,     0,    36,    37,     0,     0,     0,     0,     0,
       0,    38,    39,    40,    41,    28,     0,     0,     0,     0,
       0,   169,    42,    43,    44,    45,    46,    47,   170,   171,
       0,     0,    48,    49,    50,     0,     0,     0,     0,    51,
      52,     0,     0,     0,     0,     0,     0,   419,     0,     0,
       0,    53,     0,     0,     0,     0,     0,    29,    30,    31,
       0,     0,     0,    54,     0,     0,     0,     0,     0,  -319,
      32,    33,    34,    35,     0,    36,    37,     0,     0,    55,
       0,     0,     0,    38,    39,    40,    41,     0,     0,     0,
       0,     0,     0,     0,    42,    43,    44,    45,    46,    47,
       0,     0,     0,     0,    48,    49,    50,     0,     0,     0,
       0,    51,    52,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    53,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    54,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,     0,
     263,    55,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,     0,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,     0,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,     0,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,     0,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,     0,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
       0,     0,     0,     0,   439,   263,   440,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   441,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   444,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   447,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   456,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   532,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,     0,     0,     0,     0,     0,
     263,   533,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,   534,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,   535,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,   536,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,   537,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     263,   561,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
       0,     0,     0,     0,     0,   263,   579,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   585,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   586,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   590,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   600,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   263,   603,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,     0,     0,     0,   263,   282,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   263,   436,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,     0,   442,
       0,   658,   661,     0,    68,     0,   666,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   443,
       0,     0,     0,     0,     0,     0,     0,     0,    68,     0,
       0,   690,   658,     0,     0,     0,     0,     0,    69,   452,
       0,   699,   700,     0,   666,     0,   702,   703,     0,     0,
       0,     0,     0,   706,     0,     0,     0,     0,     0,   453,
     438,     0,    69,     0,    70,   690,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   725,     0,     0,
       0,     0,   457,     0,     0,     0,     0,     0,    70,     0,
       0,     0,     0,     0,     0,     0,     0,    71,     0,     0,
       0,     0,   704,    72,    73,    74,    75,    76,   -43,    77,
      78,    79,     0,    80,    81,     0,    82,    83,    84,     0,
       0,    71,     0,    85,    86,    87,     0,    72,    73,    74,
      75,    76,     0,    77,    78,    79,     0,    80,    81,     0,
      82,    83,    84,     0,     0,     0,     0,    85,    86,    87,
     263,     0,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
       0,     0,   510,   263,   459,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   263,   515,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   263,     0,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281
};

static const short yycheck[] =
{
      72,   203,   104,   219,   220,    70,    23,    25,     4,     4,
      82,   113,   227,     4,     4,   221,     4,    89,     4,     4,
     323,     4,     5,     4,     5,    15,     4,     6,   633,    15,
      15,     6,     4,     5,     4,     5,     4,     4,   624,     4,
       4,     0,    32,     4,     5,   604,    32,    32,    56,    57,
     468,    41,    42,    43,    44,    45,    56,    49,    50,   124,
     125,   647,   127,   128,    54,    55,     4,     4,   133,    54,
      55,   630,   551,     6,   126,   147,   148,    15,    15,    57,
      56,     4,     4,   126,   670,     4,     4,     5,   140,     4,
       4,     5,    57,    57,    32,    32,    15,   140,   170,   171,
      90,   173,    92,    32,    33,    34,    96,   586,     4,   181,
     100,   101,    56,    32,     4,   674,    54,    55,   126,   102,
     423,   102,   112,   728,   707,    56,   126,   717,   334,   201,
     102,    98,   102,   124,     6,   718,   127,   128,   129,    56,
     730,   102,   132,   561,    16,   141,   136,     4,   144,   144,
     140,   136,   142,   141,    37,   141,   139,   142,   141,   231,
     139,   139,   234,   235,   139,   237,   238,   139,    37,   141,
     242,   243,   244,   245,   246,   193,   194,   249,   139,    56,
     141,     4,   254,   255,   102,   603,   258,   259,   102,   139,
     140,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   428,     3,     4,   139,   139,   290,   291,
     130,   139,   141,   141,   139,   139,   139,   141,   124,    37,
     143,   127,   128,   129,   124,     3,     4,   127,   128,   129,
      37,    30,    31,   345,   346,     4,    37,     4,    37,    38,
      39,    66,    67,    68,    69,    70,    15,    37,    15,   133,
     134,   135,    51,    52,    53,    37,   331,     4,   333,    58,
     485,    37,    37,    32,    63,    32,    37,    66,    67,    68,
      69,    70,    71,   139,   139,   141,   141,    76,    77,    78,
      79,    80,    81,    82,    37,    54,    55,    54,    55,     4,
      89,   124,    91,    37,   127,   128,   129,   120,   121,   122,
      15,   527,   139,    37,   141,   530,    37,   106,   131,    37,
      57,    39,   139,   112,   141,   138,   141,    32,    37,     4,
       4,     4,   349,    70,     4,     4,     4,    96,    75,     4,
       4,     4,   414,   415,   416,   417,     3,    57,   137,    54,
      55,     4,   141,     4,   108,   144,   145,    94,    30,    31,
      32,    33,    34,   100,   101,   567,     4,   439,    16,    16,
     442,   443,    57,   445,     4,   112,   140,   136,   140,   136,
     452,   453,    37,     4,     4,     4,     4,   459,     4,   407,
       4,     3,     4,    15,     6,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     4,     4,     4,    56,   139,    30,    31,
      37,    56,    37,   139,    37,    37,    38,    39,    37,    37,
      37,   136,    37,    37,    37,    37,    37,   509,    37,    51,
      52,    53,    37,    37,    37,    37,    58,   139,   139,    37,
      37,    63,    37,   555,    37,     4,   140,    57,   140,    71,
     141,   141,   141,   141,    76,    77,    78,    79,    80,    81,
      82,    37,     6,   545,   141,   141,     6,    89,   550,    91,
     552,   141,   500,   501,     4,   587,     4,     3,     4,   141,
       6,   563,   141,     4,   106,     4,     4,     4,     4,     4,
     112,     4,     4,     4,     4,     4,     4,   140,   140,   581,
       4,     4,    37,    37,    30,    31,   588,   733,   141,   141,
     736,    37,    38,    39,   740,   137,     4,   139,   139,    56,
      16,   141,   144,   145,     4,    51,    52,    53,     4,    16,
      37,    37,    58,   141,   141,   141,    98,    63,   650,    16,
      88,     4,     4,   139,    89,    71,    57,   141,    16,   141,
      76,    77,    78,    79,    80,    81,    82,     3,     4,    70,
     141,   141,   141,    89,    75,    91,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   140,    57,   141,   141,    37,
     106,   141,   664,    94,    30,    31,   112,    38,    37,   100,
     101,    37,    38,    39,    40,    56,    37,    37,   141,     6,
     139,   112,   140,    37,    99,    51,    52,    53,   140,    91,
      57,   137,    58,   139,    37,     6,    37,    63,   144,   145,
      16,    56,    37,    56,    16,    71,    37,     3,     4,    37,
      76,    77,    78,    79,    80,    81,    82,    37,    37,    37,
      16,    57,    37,    89,    37,    91,    28,    29,    30,    31,
      32,    33,    34,    37,    30,    31,    37,    25,    37,    56,
     106,    37,    38,    39,    37,    37,   112,   141,   141,   141,
      37,    37,   141,     4,   141,    51,    52,    53,    98,    57,
      25,   141,    58,     4,   141,   141,   141,    63,   141,   141,
       4,   137,     4,   141,   236,    71,   141,   141,   144,   145,
      76,    77,    78,    79,    80,    81,    82,     3,     4,   141,
     141,   141,   137,    89,   549,    91,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
     106,   209,   647,   630,    30,    31,   112,   674,   680,   665,
     480,    37,    38,    39,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    51,    52,    53,   720,   693,
     517,   137,    58,     4,   548,   416,    26,    63,   144,   145,
     672,    97,    -1,   541,   233,    71,    -1,    -1,    -1,    -1,
      76,    77,    78,    79,    80,    81,    82,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    -1,    91,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    46,    47,    48,    -1,    -1,
     106,    -1,    -1,    -1,    -1,    -1,   112,    -1,    59,    60,
      61,    62,    -1,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    72,    73,    74,    75,     4,    -1,    -1,    -1,    -1,
      -1,   137,    83,    84,    85,    86,    87,    88,   144,   145,
      -1,    -1,    93,    94,    95,    -1,    -1,    -1,    -1,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      -1,   112,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,
      -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,   130,
      59,    60,    61,    62,    -1,    64,    65,    -1,    -1,   140,
      -1,    -1,    -1,    72,    73,    74,    75,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,    88,
      -1,    -1,    -1,    -1,    93,    94,    95,    -1,    -1,    -1,
      -1,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   124,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      15,   140,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    -1,    -1,    -1,   139,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
      15,   141,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   141,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   141,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   141,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   141,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   141,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    -1,    -1,    -1,    -1,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,   141,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    -1,    -1,    15,   139,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   139,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,   139,
      -1,   637,   638,    -1,     4,    -1,   642,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   139,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,
      -1,   667,   668,    -1,    -1,    -1,    -1,    -1,    38,   139,
      -1,   677,   678,    -1,   680,    -1,   682,   683,    -1,    -1,
      -1,    -1,    -1,   689,    -1,    -1,    -1,    -1,    -1,   139,
      36,    -1,    38,    -1,    64,   701,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   713,    -1,    -1,
      -1,    -1,   139,    -1,    -1,    -1,    -1,    -1,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,
      -1,    -1,   139,   103,   104,   105,   106,   107,   108,   109,
     110,   111,    -1,   113,   114,    -1,   116,   117,   118,    -1,
      -1,    97,    -1,   123,   124,   125,    -1,   103,   104,   105,
     106,   107,    -1,   109,   110,   111,    -1,   113,   114,    -1,
     116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    -1,    37,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    15,    -1,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   120,   121,   122,   131,   138,   147,   163,   164,   151,
     152,   149,   150,   256,   257,   251,   252,     0,   165,   153,
       4,    56,   126,   260,   261,    56,   253,   254,     4,    46,
      47,    48,    59,    60,    61,    62,    64,    65,    72,    73,
      74,    75,    83,    84,    85,    86,    87,    88,    93,    94,
      95,   100,   101,   112,   124,   140,   166,   173,   175,   193,
     195,   204,   205,   207,   209,   243,   258,   259,     4,    38,
      64,    97,   103,   104,   105,   106,   107,   109,   110,   111,
     113,   114,   116,   117,   118,   123,   124,   125,   154,     6,
       4,   124,   127,   128,   129,   263,   264,    56,   261,   128,
     129,   255,   264,   254,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   191,    56,    56,    49,    50,    37,    37,
       4,   148,    56,    37,    37,    37,    37,    37,    37,    37,
      37,    37,   167,    37,    37,    37,   176,    37,   139,   140,
     192,   130,     4,   148,     4,     3,     4,    30,    31,    37,
      38,    39,    51,    52,    53,    58,    63,    71,    76,    77,
      78,    79,    80,    81,    82,    89,    91,   106,   112,   137,
     144,   145,   213,     4,     4,   158,     4,   157,   156,     4,
       4,     4,   213,     4,     3,     4,   159,   160,   161,     4,
     108,   213,     4,    16,    16,    57,   140,   263,    57,   140,
     211,   212,   211,   174,   244,     4,     4,     4,     4,   168,
       4,   197,     4,   148,   148,     4,   148,   141,   148,   206,
     208,     4,   210,     4,     5,   102,   169,    37,   148,     4,
       4,    37,   160,    56,     6,   139,   155,     6,   139,   213,
     213,   213,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,   213,   213,    15,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   139,     6,   139,   213,   139,   139,     4,   139,
       6,   139,   213,   139,   139,     4,   162,    56,   264,   264,
     140,     4,   124,   127,   128,   129,    57,   140,   213,     4,
      57,    70,    94,   175,   219,     4,    57,   245,   139,   141,
     141,   165,   198,   196,   141,   141,   141,   141,   141,   139,
     141,   194,   141,   194,   139,   210,   141,    37,     4,     5,
     102,   139,   141,   169,   141,     6,     6,   213,   141,   260,
     213,   213,   153,   213,   213,   141,   213,   213,   213,   213,
     213,     4,     4,   213,     4,     4,     4,     4,   213,   213,
       4,     4,   213,   213,     4,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,     4,     4,     4,
     213,   213,     3,     4,     4,     4,   139,   266,   140,   140,
       4,   126,   140,   262,   220,   225,   230,   246,     4,    36,
      37,   201,    57,   139,   197,   148,   148,   210,   170,    37,
       4,     5,   102,   141,   211,   211,   139,    57,    36,   139,
     141,   141,   139,   139,   141,   139,   141,   141,   141,   141,
     141,   141,   139,   139,   141,   141,   141,   139,   141,    16,
       4,   264,   129,    56,   126,   140,    37,    40,   213,   233,
     234,    16,   213,   235,   234,   213,   248,   139,     4,   144,
     202,   203,    16,   197,   169,   172,    37,   141,   141,     4,
     213,   213,   213,   213,   213,   213,     4,   213,   140,   267,
      16,   265,    66,    67,    68,    69,    70,   141,   232,    37,
      37,   233,    16,    98,   215,    16,    88,   236,   231,     4,
      98,   249,     4,     4,   141,   203,    89,   199,   141,   169,
     171,   141,   141,   141,   141,   141,   141,   141,   141,    57,
     264,   264,   141,   213,    16,    37,    38,   216,   215,    56,
      37,   250,    37,   247,   141,     6,   194,   141,   169,   140,
     267,   141,   213,    37,    99,   217,   217,   174,   213,   249,
     213,   140,   211,    91,   200,   141,    57,    39,   233,   141,
     213,    37,   221,   226,    57,   141,   141,     6,    37,    16,
     141,   213,   133,   134,   135,   218,    56,   249,   211,   213,
     141,    56,   227,   141,   222,   239,   233,     4,    15,    32,
      41,    42,    43,    44,    45,    54,    55,    90,    92,    96,
     112,   132,   136,   140,   142,   177,   178,   181,   182,   184,
     186,   187,   188,   193,     4,    57,    16,    37,    37,    37,
     185,    37,    37,     4,    54,   178,   180,    37,   184,    57,
      37,   192,   240,   228,    54,    55,    96,   136,   177,    54,
      55,   177,   189,   211,    37,   183,   177,   179,    37,   143,
     194,   180,   223,   211,    56,    25,   237,    37,    37,   141,
      37,   141,    37,    37,   141,   141,   213,     4,   181,   141,
     177,   178,   141,   237,   141,   187,     4,    98,   214,   177,
     177,   179,   177,   177,   139,   141,   177,   214,    57,    25,
     238,   141,   141,   141,   141,   141,     4,   238,   241,     4,
       6,    16,   190,   141,   141,   177,   141,   141,   141,   190,
     238,   189,     4,   229,   141,   192,   224,   190,   194,   194,
     242,   194
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 8:
#line 172 "ldgram.y"
    { ldlex_defsym(); }
    break;

  case 9:
#line 174 "ldgram.y"
    {
		  ldlex_popstate();
		  lang_add_assignment(exp_assop(yyvsp[-1].token,yyvsp[-2].name,yyvsp[0].etree));
		}
    break;

  case 10:
#line 182 "ldgram.y"
    {
		  ldlex_mri_script ();
		  PUSH_ERROR (_("MRI style script"));
		}
    break;

  case 11:
#line 187 "ldgram.y"
    {
		  ldlex_popstate ();
		  mri_draw_tree ();
		  POP_ERROR ();
		}
    break;

  case 16:
#line 202 "ldgram.y"
    {
			einfo(_("%P%F: unrecognised keyword in MRI style script '%s'\n"),yyvsp[0].name);
			}
    break;

  case 17:
#line 205 "ldgram.y"
    {
			config.map_filename = "-";
			}
    break;

  case 20:
#line 211 "ldgram.y"
    { mri_public(yyvsp[-2].name, yyvsp[0].etree); }
    break;

  case 21:
#line 213 "ldgram.y"
    { mri_public(yyvsp[-2].name, yyvsp[0].etree); }
    break;

  case 22:
#line 215 "ldgram.y"
    { mri_public(yyvsp[-1].name, yyvsp[0].etree); }
    break;

  case 23:
#line 217 "ldgram.y"
    { mri_format(yyvsp[0].name); }
    break;

  case 24:
#line 219 "ldgram.y"
    { mri_output_section(yyvsp[-2].name, yyvsp[0].etree);}
    break;

  case 25:
#line 221 "ldgram.y"
    { mri_output_section(yyvsp[-1].name, yyvsp[0].etree);}
    break;

  case 26:
#line 223 "ldgram.y"
    { mri_output_section(yyvsp[-2].name, yyvsp[0].etree);}
    break;

  case 27:
#line 225 "ldgram.y"
    { mri_align(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 28:
#line 227 "ldgram.y"
    { mri_align(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 29:
#line 229 "ldgram.y"
    { mri_alignmod(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 30:
#line 231 "ldgram.y"
    { mri_alignmod(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 33:
#line 235 "ldgram.y"
    { mri_name(yyvsp[0].name); }
    break;

  case 34:
#line 237 "ldgram.y"
    { mri_alias(yyvsp[-2].name,yyvsp[0].name,0);}
    break;

  case 35:
#line 239 "ldgram.y"
    { mri_alias (yyvsp[-2].name, 0, (int) yyvsp[0].bigint.integer); }
    break;

  case 36:
#line 241 "ldgram.y"
    { mri_base(yyvsp[0].etree); }
    break;

  case 37:
#line 243 "ldgram.y"
    { mri_truncate ((unsigned int) yyvsp[0].bigint.integer); }
    break;

  case 40:
#line 247 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file(yyvsp[0].name); }
    break;

  case 41:
#line 249 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 42:
#line 251 "ldgram.y"
    { lang_add_entry (yyvsp[0].name, FALSE); }
    break;

  case 44:
#line 256 "ldgram.y"
    { mri_order(yyvsp[0].name); }
    break;

  case 45:
#line 257 "ldgram.y"
    { mri_order(yyvsp[0].name); }
    break;

  case 47:
#line 263 "ldgram.y"
    { mri_load(yyvsp[0].name); }
    break;

  case 48:
#line 264 "ldgram.y"
    { mri_load(yyvsp[0].name); }
    break;

  case 49:
#line 269 "ldgram.y"
    { mri_only_load(yyvsp[0].name); }
    break;

  case 50:
#line 271 "ldgram.y"
    { mri_only_load(yyvsp[0].name); }
    break;

  case 51:
#line 275 "ldgram.y"
    { yyval.name = NULL; }
    break;

  case 54:
#line 282 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 55:
#line 284 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 56:
#line 288 "ldgram.y"
    { ldlang_add_undef (yyvsp[0].name); }
    break;

  case 57:
#line 290 "ldgram.y"
    { ldlang_add_undef (yyvsp[0].name); }
    break;

  case 58:
#line 292 "ldgram.y"
    { ldlang_add_undef (yyvsp[0].name); }
    break;

  case 59:
#line 296 "ldgram.y"
    { ldlex_both(); }
    break;

  case 60:
#line 298 "ldgram.y"
    { ldlex_popstate(); }
    break;

  case 73:
#line 319 "ldgram.y"
    { lang_add_target(yyvsp[-1].name); }
    break;

  case 74:
#line 321 "ldgram.y"
    { ldfile_add_library_path (yyvsp[-1].name, FALSE); }
    break;

  case 75:
#line 323 "ldgram.y"
    { lang_add_output(yyvsp[-1].name, 1); }
    break;

  case 76:
#line 325 "ldgram.y"
    { lang_add_output_format (yyvsp[-1].name, (char *) NULL,
					    (char *) NULL, 1); }
    break;

  case 77:
#line 328 "ldgram.y"
    { lang_add_output_format (yyvsp[-5].name, yyvsp[-3].name, yyvsp[-1].name, 1); }
    break;

  case 78:
#line 330 "ldgram.y"
    { ldfile_set_output_arch (yyvsp[-1].name, bfd_arch_unknown); }
    break;

  case 79:
#line 332 "ldgram.y"
    { command_line.force_common_definition = TRUE ; }
    break;

  case 80:
#line 334 "ldgram.y"
    { command_line.inhibit_common_definition = TRUE ; }
    break;

  case 82:
#line 337 "ldgram.y"
    { lang_enter_group (); }
    break;

  case 83:
#line 339 "ldgram.y"
    { lang_leave_group (); }
    break;

  case 84:
#line 341 "ldgram.y"
    { lang_add_map(yyvsp[-1].name); }
    break;

  case 85:
#line 343 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file(yyvsp[0].name); }
    break;

  case 86:
#line 345 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 87:
#line 347 "ldgram.y"
    {
		  lang_add_nocrossref (yyvsp[-1].nocrossref);
		}
    break;

  case 89:
#line 352 "ldgram.y"
    { lang_add_insert (yyvsp[0].name, 0); }
    break;

  case 90:
#line 354 "ldgram.y"
    { lang_add_insert (yyvsp[0].name, 1); }
    break;

  case 91:
#line 359 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 92:
#line 362 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 93:
#line 365 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 94:
#line 368 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 95:
#line 371 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 96:
#line 374 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 97:
#line 377 "ldgram.y"
    { yyval.integer = as_needed; as_needed = TRUE; }
    break;

  case 98:
#line 379 "ldgram.y"
    { as_needed = yyvsp[-2].integer; }
    break;

  case 99:
#line 381 "ldgram.y"
    { yyval.integer = as_needed; as_needed = TRUE; }
    break;

  case 100:
#line 383 "ldgram.y"
    { as_needed = yyvsp[-2].integer; }
    break;

  case 101:
#line 385 "ldgram.y"
    { yyval.integer = as_needed; as_needed = TRUE; }
    break;

  case 102:
#line 387 "ldgram.y"
    { as_needed = yyvsp[-2].integer; }
    break;

  case 107:
#line 402 "ldgram.y"
    { lang_add_entry (yyvsp[-1].name, FALSE); }
    break;

  case 109:
#line 404 "ldgram.y"
    {ldlex_expression ();}
    break;

  case 110:
#line 405 "ldgram.y"
    { ldlex_popstate ();
		  lang_add_assignment (exp_assert (yyvsp[-3].etree, yyvsp[-1].name)); }
    break;

  case 111:
#line 413 "ldgram.y"
    {
			  yyval.cname = yyvsp[0].name;
			}
    break;

  case 112:
#line 417 "ldgram.y"
    {
			  yyval.cname = "*";
			}
    break;

  case 113:
#line 421 "ldgram.y"
    {
			  yyval.cname = "?";
			}
    break;

  case 114:
#line 428 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[0].cname;
			  yyval.wildcard.sorted = none;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 115:
#line 434 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[0].cname;
			  yyval.wildcard.sorted = none;
			  yyval.wildcard.exclude_name_list = yyvsp[-2].name_list;
			}
    break;

  case 116:
#line 440 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-1].cname;
			  yyval.wildcard.sorted = by_name;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 117:
#line 446 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-1].cname;
			  yyval.wildcard.sorted = by_alignment;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 118:
#line 452 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_name_alignment;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 119:
#line 458 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_name;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 120:
#line 464 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_alignment_name;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 121:
#line 470 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_alignment;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 122:
#line 476 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-1].cname;
			  yyval.wildcard.sorted = by_name;
			  yyval.wildcard.exclude_name_list = yyvsp[-3].name_list;
			}
    break;

  case 123:
#line 485 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = yyvsp[0].cname;
			  tmp->next = yyvsp[-1].name_list;
			  yyval.name_list = tmp;
			}
    break;

  case 124:
#line 494 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = yyvsp[0].cname;
			  tmp->next = NULL;
			  yyval.name_list = tmp;
			}
    break;

  case 125:
#line 505 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = yyvsp[-2].wildcard_list;
			  tmp->spec = yyvsp[0].wildcard;
			  yyval.wildcard_list = tmp;
			}
    break;

  case 126:
#line 514 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = NULL;
			  tmp->spec = yyvsp[0].wildcard;
			  yyval.wildcard_list = tmp;
			}
    break;

  case 127:
#line 525 "ldgram.y"
    {
			  struct wildcard_spec tmp;
			  tmp.name = yyvsp[0].name;
			  tmp.exclude_name_list = NULL;
			  tmp.sorted = none;
			  lang_add_wild (&tmp, NULL, ldgram_had_keep);
			}
    break;

  case 128:
#line 533 "ldgram.y"
    {
			  lang_add_wild (NULL, yyvsp[-1].wildcard_list, ldgram_had_keep);
			}
    break;

  case 129:
#line 537 "ldgram.y"
    {
			  lang_add_wild (&yyvsp[-3].wildcard, yyvsp[-1].wildcard_list, ldgram_had_keep);
			}
    break;

  case 131:
#line 545 "ldgram.y"
    { ldgram_had_keep = TRUE; }
    break;

  case 132:
#line 547 "ldgram.y"
    { ldgram_had_keep = FALSE; }
    break;

  case 134:
#line 553 "ldgram.y"
    {
 		lang_add_attribute(lang_object_symbols_statement_enum);
	      	}
    break;

  case 136:
#line 558 "ldgram.y"
    {

		  lang_add_attribute(lang_constructors_statement_enum);
		}
    break;

  case 137:
#line 563 "ldgram.y"
    {
		  constructors_sorted = TRUE;
		  lang_add_attribute (lang_constructors_statement_enum);
		}
    break;

  case 139:
#line 569 "ldgram.y"
    {
			  lang_add_data ((int) yyvsp[-3].integer, yyvsp[-1].etree);
			}
    break;

  case 140:
#line 574 "ldgram.y"
    {
			  lang_add_fill (yyvsp[-1].fill);
			}
    break;

  case 141:
#line 577 "ldgram.y"
    {ldlex_expression ();}
    break;

  case 142:
#line 578 "ldgram.y"
    { ldlex_popstate ();
			  lang_add_assignment (exp_assert (yyvsp[-4].etree, yyvsp[-2].name)); }
    break;

  case 147:
#line 594 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 148:
#line 596 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 149:
#line 598 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 150:
#line 600 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 151:
#line 602 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 152:
#line 607 "ldgram.y"
    {
		  yyval.fill = exp_get_fill (yyvsp[0].etree, 0, "fill value");
		}
    break;

  case 153:
#line 614 "ldgram.y"
    { yyval.fill = yyvsp[0].fill; }
    break;

  case 154:
#line 615 "ldgram.y"
    { yyval.fill = (fill_type *) 0; }
    break;

  case 155:
#line 620 "ldgram.y"
    { yyval.token = '+'; }
    break;

  case 156:
#line 622 "ldgram.y"
    { yyval.token = '-'; }
    break;

  case 157:
#line 624 "ldgram.y"
    { yyval.token = '*'; }
    break;

  case 158:
#line 626 "ldgram.y"
    { yyval.token = '/'; }
    break;

  case 159:
#line 628 "ldgram.y"
    { yyval.token = LSHIFT; }
    break;

  case 160:
#line 630 "ldgram.y"
    { yyval.token = RSHIFT; }
    break;

  case 161:
#line 632 "ldgram.y"
    { yyval.token = '&'; }
    break;

  case 162:
#line 634 "ldgram.y"
    { yyval.token = '|'; }
    break;

  case 165:
#line 644 "ldgram.y"
    {
		  lang_add_assignment (exp_assop (yyvsp[-1].token, yyvsp[-2].name, yyvsp[0].etree));
		}
    break;

  case 166:
#line 648 "ldgram.y"
    {
		  lang_add_assignment (exp_assop ('=', yyvsp[-2].name,
						  exp_binop (yyvsp[-1].token,
							     exp_nameop (NAME,
									 yyvsp[-2].name),
							     yyvsp[0].etree)));
		}
    break;

  case 167:
#line 656 "ldgram.y"
    {
		  lang_add_assignment (exp_provide (yyvsp[-3].name, yyvsp[-1].etree, FALSE));
		}
    break;

  case 168:
#line 660 "ldgram.y"
    {
		  lang_add_assignment (exp_provide (yyvsp[-3].name, yyvsp[-1].etree, TRUE));
		}
    break;

  case 175:
#line 682 "ldgram.y"
    { region = lang_memory_region_lookup (yyvsp[0].name, TRUE); }
    break;

  case 176:
#line 685 "ldgram.y"
    {}
    break;

  case 177:
#line 690 "ldgram.y"
    {
		  region->origin = exp_get_vma (yyvsp[0].etree, 0, "origin");
		  region->current = region->origin;
		}
    break;

  case 178:
#line 698 "ldgram.y"
    {
		  region->length = exp_get_vma (yyvsp[0].etree, -1, "length");
		}
    break;

  case 179:
#line 705 "ldgram.y"
    { /* dummy action to avoid bison 1.25 error message */ }
    break;

  case 183:
#line 716 "ldgram.y"
    { lang_set_flags (region, yyvsp[0].name, 0); }
    break;

  case 184:
#line 718 "ldgram.y"
    { lang_set_flags (region, yyvsp[0].name, 1); }
    break;

  case 185:
#line 723 "ldgram.y"
    { lang_startup(yyvsp[-1].name); }
    break;

  case 187:
#line 729 "ldgram.y"
    { ldemul_hll((char *)NULL); }
    break;

  case 188:
#line 734 "ldgram.y"
    { ldemul_hll(yyvsp[0].name); }
    break;

  case 189:
#line 736 "ldgram.y"
    { ldemul_hll(yyvsp[0].name); }
    break;

  case 191:
#line 744 "ldgram.y"
    { ldemul_syslib(yyvsp[0].name); }
    break;

  case 193:
#line 750 "ldgram.y"
    { lang_float(TRUE); }
    break;

  case 194:
#line 752 "ldgram.y"
    { lang_float(FALSE); }
    break;

  case 195:
#line 757 "ldgram.y"
    {
		  yyval.nocrossref = NULL;
		}
    break;

  case 196:
#line 761 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = yyvsp[-1].name;
		  n->next = yyvsp[0].nocrossref;
		  yyval.nocrossref = n;
		}
    break;

  case 197:
#line 770 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = yyvsp[-2].name;
		  n->next = yyvsp[0].nocrossref;
		  yyval.nocrossref = n;
		}
    break;

  case 198:
#line 780 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 199:
#line 782 "ldgram.y"
    { ldlex_popstate (); yyval.etree=yyvsp[0].etree;}
    break;

  case 200:
#line 787 "ldgram.y"
    { yyval.etree = exp_unop ('-', yyvsp[0].etree); }
    break;

  case 201:
#line 789 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 202:
#line 791 "ldgram.y"
    { yyval.etree = exp_unop ((int) yyvsp[-3].integer,yyvsp[-1].etree); }
    break;

  case 203:
#line 793 "ldgram.y"
    { yyval.etree = exp_unop ('!', yyvsp[0].etree); }
    break;

  case 204:
#line 795 "ldgram.y"
    { yyval.etree = yyvsp[0].etree; }
    break;

  case 205:
#line 797 "ldgram.y"
    { yyval.etree = exp_unop ('~', yyvsp[0].etree);}
    break;

  case 206:
#line 800 "ldgram.y"
    { yyval.etree = exp_binop ('*', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 207:
#line 802 "ldgram.y"
    { yyval.etree = exp_binop ('/', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 208:
#line 804 "ldgram.y"
    { yyval.etree = exp_binop ('%', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 209:
#line 806 "ldgram.y"
    { yyval.etree = exp_binop ('+', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 210:
#line 808 "ldgram.y"
    { yyval.etree = exp_binop ('-' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 211:
#line 810 "ldgram.y"
    { yyval.etree = exp_binop (LSHIFT , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 212:
#line 812 "ldgram.y"
    { yyval.etree = exp_binop (RSHIFT , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 213:
#line 814 "ldgram.y"
    { yyval.etree = exp_binop (EQ , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 214:
#line 816 "ldgram.y"
    { yyval.etree = exp_binop (NE , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 215:
#line 818 "ldgram.y"
    { yyval.etree = exp_binop (LE , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 216:
#line 820 "ldgram.y"
    { yyval.etree = exp_binop (GE , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 217:
#line 822 "ldgram.y"
    { yyval.etree = exp_binop ('<' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 218:
#line 824 "ldgram.y"
    { yyval.etree = exp_binop ('>' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 219:
#line 826 "ldgram.y"
    { yyval.etree = exp_binop ('&' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 220:
#line 828 "ldgram.y"
    { yyval.etree = exp_binop ('^' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 221:
#line 830 "ldgram.y"
    { yyval.etree = exp_binop ('|' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 222:
#line 832 "ldgram.y"
    { yyval.etree = exp_trinop ('?' , yyvsp[-4].etree, yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 223:
#line 834 "ldgram.y"
    { yyval.etree = exp_binop (ANDAND , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 224:
#line 836 "ldgram.y"
    { yyval.etree = exp_binop (OROR , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 225:
#line 838 "ldgram.y"
    { yyval.etree = exp_nameop (DEFINED, yyvsp[-1].name); }
    break;

  case 226:
#line 840 "ldgram.y"
    { yyval.etree = exp_bigintop (yyvsp[0].bigint.integer, yyvsp[0].bigint.str); }
    break;

  case 227:
#line 842 "ldgram.y"
    { yyval.etree = exp_nameop (SIZEOF_HEADERS,0); }
    break;

  case 228:
#line 845 "ldgram.y"
    { yyval.etree = exp_nameop (ALIGNOF,yyvsp[-1].name); }
    break;

  case 229:
#line 847 "ldgram.y"
    { yyval.etree = exp_nameop (SIZEOF,yyvsp[-1].name); }
    break;

  case 230:
#line 849 "ldgram.y"
    { yyval.etree = exp_nameop (ADDR,yyvsp[-1].name); }
    break;

  case 231:
#line 851 "ldgram.y"
    { yyval.etree = exp_nameop (LOADADDR,yyvsp[-1].name); }
    break;

  case 232:
#line 853 "ldgram.y"
    { yyval.etree = exp_nameop (CONSTANT,yyvsp[-1].name); }
    break;

  case 233:
#line 855 "ldgram.y"
    { yyval.etree = exp_unop (ABSOLUTE, yyvsp[-1].etree); }
    break;

  case 234:
#line 857 "ldgram.y"
    { yyval.etree = exp_unop (ALIGN_K,yyvsp[-1].etree); }
    break;

  case 235:
#line 859 "ldgram.y"
    { yyval.etree = exp_binop (ALIGN_K,yyvsp[-3].etree,yyvsp[-1].etree); }
    break;

  case 236:
#line 861 "ldgram.y"
    { yyval.etree = exp_binop (DATA_SEGMENT_ALIGN, yyvsp[-3].etree, yyvsp[-1].etree); }
    break;

  case 237:
#line 863 "ldgram.y"
    { yyval.etree = exp_binop (DATA_SEGMENT_RELRO_END, yyvsp[-1].etree, yyvsp[-3].etree); }
    break;

  case 238:
#line 865 "ldgram.y"
    { yyval.etree = exp_unop (DATA_SEGMENT_END, yyvsp[-1].etree); }
    break;

  case 239:
#line 867 "ldgram.y"
    { /* The operands to the expression node are
			     placed in the opposite order from the way
			     in which they appear in the script as
			     that allows us to reuse more code in
			     fold_binary.  */
			  yyval.etree = exp_binop (SEGMENT_START,
					  yyvsp[-1].etree,
					  exp_nameop (NAME, yyvsp[-3].name)); }
    break;

  case 240:
#line 876 "ldgram.y"
    { yyval.etree = exp_unop (ALIGN_K,yyvsp[-1].etree); }
    break;

  case 241:
#line 878 "ldgram.y"
    { yyval.etree = exp_nameop (NAME,yyvsp[0].name); }
    break;

  case 242:
#line 880 "ldgram.y"
    { yyval.etree = exp_binop (MAX_K, yyvsp[-3].etree, yyvsp[-1].etree ); }
    break;

  case 243:
#line 882 "ldgram.y"
    { yyval.etree = exp_binop (MIN_K, yyvsp[-3].etree, yyvsp[-1].etree ); }
    break;

  case 244:
#line 884 "ldgram.y"
    { yyval.etree = exp_assert (yyvsp[-3].etree, yyvsp[-1].name); }
    break;

  case 245:
#line 886 "ldgram.y"
    { yyval.etree = exp_nameop (ORIGIN, yyvsp[-1].name); }
    break;

  case 246:
#line 888 "ldgram.y"
    { yyval.etree = exp_nameop (LENGTH, yyvsp[-1].name); }
    break;

  case 247:
#line 893 "ldgram.y"
    { yyval.name = yyvsp[0].name; }
    break;

  case 248:
#line 894 "ldgram.y"
    { yyval.name = 0; }
    break;

  case 249:
#line 898 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 250:
#line 899 "ldgram.y"
    { yyval.etree = 0; }
    break;

  case 251:
#line 903 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 252:
#line 904 "ldgram.y"
    { yyval.etree = 0; }
    break;

  case 253:
#line 908 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 254:
#line 909 "ldgram.y"
    { yyval.etree = 0; }
    break;

  case 255:
#line 913 "ldgram.y"
    { yyval.token = ONLY_IF_RO; }
    break;

  case 256:
#line 914 "ldgram.y"
    { yyval.token = ONLY_IF_RW; }
    break;

  case 257:
#line 915 "ldgram.y"
    { yyval.token = SPECIAL; }
    break;

  case 258:
#line 916 "ldgram.y"
    { yyval.token = 0; }
    break;

  case 259:
#line 919 "ldgram.y"
    { ldlex_expression(); }
    break;

  case 260:
#line 923 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 261:
#line 926 "ldgram.y"
    {
			  lang_enter_output_section_statement(yyvsp[-8].name, yyvsp[-6].etree,
							      sectype,
							      yyvsp[-4].etree, yyvsp[-3].etree, yyvsp[-5].etree, yyvsp[-1].token);
			}
    break;

  case 262:
#line 932 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 263:
#line 934 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_leave_output_section_statement (yyvsp[0].fill, yyvsp[-3].name, yyvsp[-1].section_phdr, yyvsp[-2].name);
		}
    break;

  case 264:
#line 939 "ldgram.y"
    {}
    break;

  case 265:
#line 941 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 266:
#line 943 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 267:
#line 945 "ldgram.y"
    {
			  lang_enter_overlay (yyvsp[-5].etree, yyvsp[-2].etree);
			}
    break;

  case 268:
#line 950 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 269:
#line 952 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay (yyvsp[-11].etree, (int) yyvsp[-12].integer,
					      yyvsp[0].fill, yyvsp[-3].name, yyvsp[-1].section_phdr, yyvsp[-2].name);
			}
    break;

  case 271:
#line 962 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 272:
#line 964 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_add_assignment (exp_assop ('=', ".", yyvsp[0].etree));
		}
    break;

  case 274:
#line 972 "ldgram.y"
    { sectype = noload_section; }
    break;

  case 275:
#line 973 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 276:
#line 974 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 277:
#line 975 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 278:
#line 976 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 280:
#line 981 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 281:
#line 982 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 282:
#line 986 "ldgram.y"
    { yyval.etree = yyvsp[-2].etree; }
    break;

  case 283:
#line 987 "ldgram.y"
    { yyval.etree = (etree_type *)NULL;  }
    break;

  case 284:
#line 992 "ldgram.y"
    { yyval.etree = yyvsp[-3].etree; }
    break;

  case 285:
#line 994 "ldgram.y"
    { yyval.etree = yyvsp[-7].etree; }
    break;

  case 286:
#line 998 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 287:
#line 999 "ldgram.y"
    { yyval.etree = (etree_type *) NULL;  }
    break;

  case 288:
#line 1004 "ldgram.y"
    { yyval.integer = 0; }
    break;

  case 289:
#line 1006 "ldgram.y"
    { yyval.integer = 1; }
    break;

  case 290:
#line 1011 "ldgram.y"
    { yyval.name = yyvsp[0].name; }
    break;

  case 291:
#line 1012 "ldgram.y"
    { yyval.name = DEFAULT_MEMORY_REGION; }
    break;

  case 292:
#line 1017 "ldgram.y"
    {
		  yyval.section_phdr = NULL;
		}
    break;

  case 293:
#line 1021 "ldgram.y"
    {
		  struct lang_output_section_phdr_list *n;

		  n = ((struct lang_output_section_phdr_list *)
		       xmalloc (sizeof *n));
		  n->name = yyvsp[0].name;
		  n->used = FALSE;
		  n->next = yyvsp[-2].section_phdr;
		  yyval.section_phdr = n;
		}
    break;

  case 295:
#line 1037 "ldgram.y"
    {
			  ldlex_script ();
			  lang_enter_overlay_section (yyvsp[0].name);
			}
    break;

  case 296:
#line 1042 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 297:
#line 1044 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay_section (yyvsp[0].fill, yyvsp[-1].section_phdr);
			}
    break;

  case 302:
#line 1061 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 303:
#line 1062 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 304:
#line 1064 "ldgram.y"
    {
		  lang_new_phdr (yyvsp[-5].name, yyvsp[-3].etree, yyvsp[-2].phdr.filehdr, yyvsp[-2].phdr.phdrs, yyvsp[-2].phdr.at,
				 yyvsp[-2].phdr.flags);
		}
    break;

  case 305:
#line 1072 "ldgram.y"
    {
		  yyval.etree = yyvsp[0].etree;

		  if (yyvsp[0].etree->type.node_class == etree_name
		      && yyvsp[0].etree->type.node_code == NAME)
		    {
		      const char *s;
		      unsigned int i;
		      static const char * const phdr_types[] =
			{
			  "PT_NULL", "PT_LOAD", "PT_DYNAMIC",
			  "PT_INTERP", "PT_NOTE", "PT_SHLIB",
			  "PT_PHDR", "PT_TLS"
			};

		      s = yyvsp[0].etree->name.name;
		      for (i = 0;
			   i < sizeof phdr_types / sizeof phdr_types[0];
			   i++)
			if (strcmp (s, phdr_types[i]) == 0)
			  {
			    yyval.etree = exp_intop (i);
			    break;
			  }
		      if (i == sizeof phdr_types / sizeof phdr_types[0])
			{
			  if (strcmp (s, "PT_GNU_EH_FRAME") == 0)
			    yyval.etree = exp_intop (0x6474e550);
			  else if (strcmp (s, "PT_GNU_STACK") == 0)
			    yyval.etree = exp_intop (0x6474e551);
			  else
			    {
			      einfo (_("\
%X%P:%S: unknown phdr type `%s' (try integer literal)\n"),
				     s);
			      yyval.etree = exp_intop (0);
			    }
			}
		    }
		}
    break;

  case 306:
#line 1116 "ldgram.y"
    {
		  memset (&yyval.phdr, 0, sizeof (struct phdr_info));
		}
    break;

  case 307:
#line 1120 "ldgram.y"
    {
		  yyval.phdr = yyvsp[0].phdr;
		  if (strcmp (yyvsp[-2].name, "FILEHDR") == 0 && yyvsp[-1].etree == NULL)
		    yyval.phdr.filehdr = TRUE;
		  else if (strcmp (yyvsp[-2].name, "PHDRS") == 0 && yyvsp[-1].etree == NULL)
		    yyval.phdr.phdrs = TRUE;
		  else if (strcmp (yyvsp[-2].name, "FLAGS") == 0 && yyvsp[-1].etree != NULL)
		    yyval.phdr.flags = yyvsp[-1].etree;
		  else
		    einfo (_("%X%P:%S: PHDRS syntax error at `%s'\n"), yyvsp[-2].name);
		}
    break;

  case 308:
#line 1132 "ldgram.y"
    {
		  yyval.phdr = yyvsp[0].phdr;
		  yyval.phdr.at = yyvsp[-2].etree;
		}
    break;

  case 309:
#line 1140 "ldgram.y"
    {
		  yyval.etree = NULL;
		}
    break;

  case 310:
#line 1144 "ldgram.y"
    {
		  yyval.etree = yyvsp[-1].etree;
		}
    break;

  case 311:
#line 1150 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("dynamic list"));
		}
    break;

  case 312:
#line 1155 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 316:
#line 1172 "ldgram.y"
    {
		  lang_append_dynamic_list (yyvsp[-1].versyms);
		}
    break;

  case 317:
#line 1180 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("VERSION script"));
		}
    break;

  case 318:
#line 1185 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 319:
#line 1194 "ldgram.y"
    {
		  ldlex_version_script ();
		}
    break;

  case 320:
#line 1198 "ldgram.y"
    {
		  ldlex_popstate ();
		}
    break;

  case 323:
#line 1210 "ldgram.y"
    {
		  lang_register_vers_node (NULL, yyvsp[-2].versnode, NULL);
		}
    break;

  case 324:
#line 1214 "ldgram.y"
    {
		  lang_register_vers_node (yyvsp[-4].name, yyvsp[-2].versnode, NULL);
		}
    break;

  case 325:
#line 1218 "ldgram.y"
    {
		  lang_register_vers_node (yyvsp[-5].name, yyvsp[-3].versnode, yyvsp[-1].deflist);
		}
    break;

  case 326:
#line 1225 "ldgram.y"
    {
		  yyval.deflist = lang_add_vers_depend (NULL, yyvsp[0].name);
		}
    break;

  case 327:
#line 1229 "ldgram.y"
    {
		  yyval.deflist = lang_add_vers_depend (yyvsp[-1].deflist, yyvsp[0].name);
		}
    break;

  case 328:
#line 1236 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (NULL, NULL);
		}
    break;

  case 329:
#line 1240 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (yyvsp[-1].versyms, NULL);
		}
    break;

  case 330:
#line 1244 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (yyvsp[-1].versyms, NULL);
		}
    break;

  case 331:
#line 1248 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (NULL, yyvsp[-1].versyms);
		}
    break;

  case 332:
#line 1252 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (yyvsp[-5].versyms, yyvsp[-1].versyms);
		}
    break;

  case 333:
#line 1259 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, yyvsp[0].name, ldgram_vers_current_lang, FALSE);
		}
    break;

  case 334:
#line 1263 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, yyvsp[0].name, ldgram_vers_current_lang, TRUE);
		}
    break;

  case 335:
#line 1267 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, yyvsp[0].name, ldgram_vers_current_lang, FALSE);
		}
    break;

  case 336:
#line 1271 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, yyvsp[0].name, ldgram_vers_current_lang, TRUE);
		}
    break;

  case 337:
#line 1275 "ldgram.y"
    {
			  yyval.name = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = yyvsp[-1].name;
			}
    break;

  case 338:
#line 1280 "ldgram.y"
    {
			  struct bfd_elf_version_expr *pat;
			  for (pat = yyvsp[-2].versyms; pat->next != NULL; pat = pat->next);
			  pat->next = yyvsp[-8].versyms;
			  yyval.versyms = yyvsp[-2].versyms;
			  ldgram_vers_current_lang = yyvsp[-3].name;
			}
    break;

  case 339:
#line 1288 "ldgram.y"
    {
			  yyval.name = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = yyvsp[-1].name;
			}
    break;

  case 340:
#line 1293 "ldgram.y"
    {
			  yyval.versyms = yyvsp[-2].versyms;
			  ldgram_vers_current_lang = yyvsp[-3].name;
			}
    break;

  case 341:
#line 1298 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 342:
#line 1302 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 343:
#line 1306 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 344:
#line 1310 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 345:
#line 1314 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, "extern", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 346:
#line 1318 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, "extern", ldgram_vers_current_lang, FALSE);
		}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 3880 "ldgram.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1328 "ldgram.y"

void
yyerror(arg)
     const char *arg;
{
  if (ldfile_assumed_script)
    einfo (_("%P:%s: file format not recognized; treating as linker script\n"),
	   ldfile_input_filename);
  if (error_index > 0 && error_index < ERROR_NAME_MAX)
     einfo ("%P%F:%S: %s in %s\n", arg, error_names[error_index-1]);
  else
     einfo ("%P%F:%S: %s\n", arg);
}

