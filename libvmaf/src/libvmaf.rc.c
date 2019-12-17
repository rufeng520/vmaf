#include <errno.h>
#include <stdlib.h>

#include "feature/feature_extractor.h"
#include "feature/feature_collector.h"
#include "libvmaf/libvmaf.rc.h"
#include "model.h"

typedef struct {
    VmafFeatureCollector **fex;
    unsigned cnt, capacity;
} FeatureExtractorVector;

typedef struct VmafContext {
    VmafConfiguration cfg;
    VmafFeatureCollector *feature_collector;
    FeatureExtractorVector registered_feature_extractors;
} VmafContext;

static int feature_extractor_vector_init(FeatureExtractorVector *fev)
{
    fev->cnt = 0;
    fev->capacity = 8;
    size_t sz = sizeof(*(fev->fex)) * fev->capacity;
    fev->fex = malloc(sz);
    if (!fev->fex) return -ENOMEM;
    memset(fev->fex, 0, sz);
    return 0;
}

static int feature_extractor_vector_append(FeatureExtractorVector *fev,
                                           VmafFeatureExtractor *fex)
{
    if (!fev) return -EINVAL;
    if (!fex) return -EINVAL;

    if (fev->cnt >= fev->capacity) {
        size_t capacity = fev->capacity * 2;
        VmafFeatureExtractor **fex =
            realloc(fev->fex, sizeof(*(fev->fex)) * capacity);
        if (!fex) return -ENOMEM;
        fev->fex = fex;
        fev->capacity = capacity;
        for (int i = fev->cnt; i < fev->capacity; i++)
            fev->fex[i] = NULL;
    }

    fev->fex[fev->cnt++] = fex;
    return 0;
}

static void feature_extractor_vector_destroy(FeatureExtractorVector *fev)
{
    if (!fev) return;
    if (!fev->fex) return;
    free(fev->fex);
    return;
}

void vmaf_default_configuration(VmafConfiguration *cfg)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
}

int vmaf_init(VmafContext **vmaf, VmafConfiguration cfg)
{
    if (!vmaf) return -EINVAL;

    int err = 0;

    VmafContext *const v = *vmaf = malloc(sizeof(*v));
    if (!v) goto fail;
    v->cfg = cfg;
    err = vmaf_feature_collector_init(&(v->feature_collector));
    if (err) goto free_v;
    err = feature_extractor_vector_init(&(v->registered_feature_extractors));
    if (err) goto free_feature_collector;

    return 0;

free_feature_collector:
    vmaf_feature_collector_destroy(v->feature_collector);
free_v:
    free(v);
fail:
    return -ENOMEM;
}

int vmaf_close(VmafContext *vmaf)
{
    if (!vmaf) return -EINVAL;

    feature_extractor_vector_destroy(&(vmaf->registered_feature_extractors));
    vmaf_feature_collector_destroy(vmaf->feature_collector);
    free(vmaf);

    return 0;
}

int vmaf_import_feature_score(VmafContext *vmaf, char *feature_name,
                              double value, unsigned index)
{
    if (!vmaf) return -EINVAL;
    if (!feature_name) return -EINVAL;
    return vmaf_feature_collector_append(vmaf->feature_collector, feature_name,
                                         value, index);
}

int vmaf_use_features_from_model(VmafContext *vmaf, VmafModel model)
{
    return 0;
}

int vmaf_use_feature(VmafContext *vmaf, const char *feature_name)
{
    return 0;
}

int vmaf_read_pictures(VmafContext *vmaf, VmafPicture *ref, VmafPicture *dist)
{
    return 0;
}

int vmaf_score_at_index(VmafContext *vmaf, VmafModel model, VmafScore *score,
                        unsigned index)
{
    return 0;
}

int vmaf_score_pooled(VmafContext *vmaf, VmafModel model,
                      enum VmafPoolingMethod pool_method, VmafScore *score)
{
    return 0;
}
