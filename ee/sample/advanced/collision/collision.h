/*gjk_object types */
#define GJK_MESH 0
#define GJK_BOX 1
#define GJK_SPHERE 2
#define GJK_CONE 3
#define GJK_CYLINDER 4

/*gjk_object */
typedef struct {
  /* for box,sphere etc.. */
  u_int type; 
  float radius,half_length; /* cylinder,shpere,cone */
  sceVu0FVECTOR half_box; /* box */

  sceVu0FMATRIX *lw; /*local world matrix */
  sceVu0FMATRIX *lwrot; /* local world wo translation */
  sceVu0FVECTOR *vp; /* vertex pointer */
  u_int **neighbors; /* neighbors index pointer */
  u_int *nneighbors;/* number of neighbors */
} GJKObj;

/* simplex definition */
typedef struct {
  /* num of Points */
  u_char Wk; 

  /* precission */
  float epsilon;

  /* objA */
  sceVu0FVECTOR closest_pointA;
  sceVu0FVECTOR support_pointsA[4];
  u_int hintA;
  
  /* objB */
  sceVu0FVECTOR closest_pointB;
  sceVu0FVECTOR support_pointsB[4];
  u_int hintB;

  /* cache last*/
  u_int cache_valid;
  sceVu0FVECTOR cache_vect;
} Simplex;

static inline float my_sqrtf(float arg)
{
        float result;
        asm ("sqrt.s %0,%1" : "=f" (result) : "f" (arg): "cc");
        return result;
}
 
static inline float my_rsqrtf(float arg1,float arg2)
{
    float result;
    asm  ("rsqrt.s %0,%1,%2" : "=f" (result) : "f" (arg1),"f"(arg2) : "cc");
    return result;
}      

extern void collision_init();
extern int  is_collision(Simplex *simplex,GJKObj *gjkobjA,GJKObj *gjkobjB);
extern float calc_dist(Simplex *simplex,GJKObj *gjkobjA,GJKObj *gjkobjB,u_int calc_points);

extern int alloc_gjkobj_neighbors(void *gjk_data_addr,GJKObj *gjkobj);
extern void free_gjkobj_neighbors(GJKObj *gjkobj); 
