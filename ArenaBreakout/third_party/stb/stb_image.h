#ifndef STB_IMAGE_H
#define STB_IMAGE_H

/* Simplified stb_image stub for compilation */

#ifdef __cplusplus
extern "C" {
#endif

unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
    (void)filename; (void)x; (void)y; (void)comp; (void)req_comp;
    return 0;
}

void stbi_image_free(void *retval_from_stbi_load) {
    (void)retval_from_stbi_load;
}

#ifdef __cplusplus
}
#endif

#endif /* STB_IMAGE_H */
