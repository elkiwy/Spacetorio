#include "Components_planet.hpp"
#include "Components_clickables.hpp"
#include "Scene.hpp"
#include "SceneBiome.hpp"
#include "Universe.hpp"
#include "SDL_pixels.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "Utils_math.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>


SDL_Color getPixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + (y * surface->pitch) + x * bpp; /* Here p is the address to the pixel we want to retrieve */
    assert((bpp == 4) && "ERROR: using getPixel with a bpp different than 4! Look at https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface");
    Uint32 val = *(Uint32 *)p;
    SDL_Color rgb;
    SDL_GetRGB(val, surface->format, &rgb.r, &rgb.g, &rgb.b);
    return rgb;
}

iPoint dstToSrc(iPoint dst, int Ro, int Ri, iSize srcSize, float minAngle, float maxAngle){
    const float angle = atan2(dst.y-Ro, dst.x-Ro);
    if (angle > maxAngle || angle < minAngle){ return {-1, -1};}
    const float percAngle = (angle-minAngle) / (maxAngle-minAngle);
    const int distance = sqrt((dst.y-Ro)*(dst.y-Ro) + (dst.x-Ro)*(dst.x-Ro));
    const float percHeight = ((float)(distance-Ri)) / ((float)(Ro-Ri));
    const int targetSrcHeight = floor(percHeight * ((float)(srcSize.h)));
    const float percWidth = angle / (M_PI*2.0f);
    return {(int)(srcSize.w*percAngle) % srcSize.w, srcSize.h - targetSrcHeight - 1};
}

SDL_Surface* bendImageOnArc(SDL_Surface* src, float angle, int Ro, int Ri){
    const iSize dstSize = {Ro*2, Ro*2};
    SDL_Surface* dstSurface = SDL_CreateRGBSurface(0, dstSize.w, dstSize.h, 32, 0, 0, 0, 0);
    Uint8* dstPixels = (Uint8*)dstSurface->pixels;

    for (int i=0; i<Ro; i++){
        const int outer_radius = sqrt(Ro*Ro - i*i)-1;
        for(int j=-outer_radius; j<outer_radius; j++){
            const int inner_radius = (i<Ri) ? sqrt(Ri*Ri - i*i) : -1;
            if (j > inner_radius || j < -inner_radius){
                iPoint dstPoint = { Ro + j, Ro - i };
                iPoint srcPoint = dstToSrc(dstPoint, Ro, Ri, { src->w, src->h }, deg2rad(-(angle / 2.0f) - 90.0f), deg2rad((angle / 2.0f) - 90.0f));
                if (srcPoint.x > -1) {
                    SDL_Color col = getPixel(src, srcPoint.x, srcPoint.y);
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 0] = col.r;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 1] = col.g;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 2] = col.b;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 3] = 255;
                }

                dstPoint = { Ro + j, Ro + i };
                srcPoint = dstToSrc(dstPoint, Ro, Ri, { src->w, src->h }, deg2rad(-(angle / 2.0f) - 90.0f), deg2rad((angle / 2.0f) - 90.0f));
                if (srcPoint.x > -1) {
                    SDL_Color col = getPixel(src, srcPoint.x, srcPoint.y);
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 0] = col.r;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 1] = col.g;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 2] = col.b;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 3] = 255;
                }


            }
        }
    }

    return dstSurface;
}




PlanetBiomeComponent::PlanetBiomeComponent(PlanetBiomeType t){

}

PlanetBiomeComponent::~PlanetBiomeComponent(){
    if (surface_space != nullptr){SDL_FreeSurface(surface_space); surface_space = nullptr;}
    if (surface_flat != nullptr){SDL_FreeSurface(surface_flat); surface_flat = nullptr;}
    if (biomeScene != nullptr){delete biomeScene; surface_flat = nullptr;}
}

SceneBiome* PlanetBiomeComponent::getBiomeScene(){
    if (biomeScene == nullptr){
        iSize sz = iSize(this->chunkData_lowRes_sizeW, this->chunkData_lowRes_sizeH);
        biomeScene = new SceneBiome();
        biomeScene->init(surface_flat);
        biomeScene->spawnPlayerAt(fPoint(sz.w*TILE_SIZE*0.5f,sz.h*TILE_SIZE*0.25f));
    std::cout << "pizzo" << std::endl;
    }
    std::cout << "cippoppa" << std::endl;
    return biomeScene;
}

void PlanetBiomeComponent::generateTerrain() {
    //Assert that I'm not doing stupid things
    assert((this->chunkData_lowRes_sizeW == 0 && this->chunkData_lowRes_sizeH == 0) && "ERROR: calling generateTerrain() on a biome with a terrain.");
    assert((this->size.w > 0.0f && this->size.h > 0.0f) && "ERROR: calling generateTerrain() on a biome without a size set first.");

    //Generate terrain
    NoiseSurfaceGenerator gen;
    NoiseSurfaceGenerator::GenerationSettings conf;
    conf.seed = randInt(0, 999999);
    std::cout << "Generating a terrain of size " << this->size.w << " " << this->size.h << std::endl;
    this->surface_flat = gen.generateTerrain({(int)size.w, (int)size.h}, 3, conf);
    this->chunkData_lowRes_sizeW = size.w;
    this->chunkData_lowRes_sizeH = size.h;
    std::cout << "Done generating terrain of size " << this->size.w << " " << this->size.h << std::endl;

    //Create the bent surface to be able to use it in space
    const float Ro = this->planetRadius + this->size.h*0.5f;
    const float Ri = this->planetRadius - this->size.h*0.5f;
    this->surface_space = bendImageOnArc(this->surface_flat, rad2deg(this->curvature), Ro, Ri);
    this->texture_space = Texture(this->surface_space);

    //DEBUG: save the images
    SDL_SaveBMP(surface_flat, "Terrain_normal.bmp");
    SDL_SaveBMP(surface_space, "Terrain_bent.bmp");
}

void PlanetBiomeComponent::setData(fSize size, float curvature, float planetRadius, float directionDeg){
    this->size = size; this->curvature = curvature;
    this->planetRadius = planetRadius;
    this->directionDeg = directionDeg;
}

void PlanetBiomeComponent::render(fPoint planetPosOnScreen, float cameraZoom) const{
    global_renderer->drawTexture(texture_space, planetPosOnScreen.x, planetPosOnScreen.y, directionDeg, cameraZoom);
}



/*
**
** Planet Component
**
*/

void PlanetComponent::addBiome(PlanetBiomeType t){
    PlanetBiome b = PlanetBiome(myEntity, BIOME_FLATS);
    biomes.emplace_back(b.enttHandle);
}

PlanetComponent::PlanetComponent(Entity planetEntity, float s):myEntity(planetEntity),planetRadius(s){
    Scene* scene = myEntity.scene;

    //Init test biomes
    addBiome(BIOME_FLATS);
    addBiome(BIOME_MOUNTAIN);
    addBiome(BIOME_LAKE);
    addBiome(BIOME_MOUNTAIN);
    addBiome(BIOME_LAKE);

    //Init all the biomes
    const int nBiomes = biomes.size();
    const float angle = deg2rad(360.0f/(float)nBiomes);
    const float biomeW = planetRadius * angle; //Arc length formula
    const int biomeH = 300;
    for(int i=0;i<biomes.size();i++){
        float direction = rad2deg(0.0f + i*angle);
        float angleToCenter = deg2rad(direction)+(angle/2.0f);

        //Setup biome
        PlanetBiome b = {biomes[i], scene};
        PlanetBiomeComponent& bc = b.getComponent<PlanetBiomeComponent>();
        fSize biomeSize = fSize(roundChunk(biomeW), roundChunk(biomeH));
        bc.setData(biomeSize, angle, planetRadius, 90.0f + rad2deg(angleToCenter));
        bc.generateTerrain();

        //Setup click interaction
        auto& clickable = b.getComponent<ClickableCircleComponent>();
        clickable.offset = fPoint(0.0f, 0.0f).movedByTo(planetRadius, angleToCenter);
        clickable.onclick = [i, &bc](){
            std::cout << "Loading scene of biome " << std::to_string(i) << std::endl;
            SceneBiome* biomeScene = bc.getBiomeScene();
            global_universe->switchScene(biomeScene);
        };
    }
}

void PlanetComponent::render(const PositionComponent& posComp, const Camera& cam) const{
    SDL_Color col = {255,255,255,255};
    auto& pos = posComp.pos;
    const float scaledRadius = planetRadius * cam.zoom;
    const fPoint planetScreenPos = cam.worldToScreen({pos.x, pos.y});
    //global_renderer->drawCircle(planetScreenPos.x, planetScreenPos.y, scaledRadius, col);

    //Render biomes guides
    const int nBiomes = biomes.size();
    const float biomesAngRad = deg2rad(360.0f/(float)nBiomes);
    const fPoint startPt = planetScreenPos + fPoint(scaledRadius, 0.0f);
    for (int i=0;i<nBiomes;i++){
        float currAng = biomesAngRad * i;
        fPoint p = startPt.rotatedPoint(currAng, planetScreenPos);
        global_renderer->drawLine(planetScreenPos.x, planetScreenPos.y, p.x, p.y, col);
    }

    //Render Biomes textures
    for(auto& biome : biomes){
        PlanetBiome b = {biome, myEntity.scene};
        auto& bc = b.getComponent<PlanetBiomeComponent>();
        bc.render(planetScreenPos, cam.zoom);
    }

    //Re-render circle for guide
    //global_renderer->drawCircle(planetScreenPos.x, planetScreenPos.y, scaledRadius, {255,0,0,255});
}
