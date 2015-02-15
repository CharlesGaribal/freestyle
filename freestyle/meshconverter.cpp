#include "meshconverter.h"
#include "timer.h"


void MeshConverter::convert(DefaultPolyMesh *in, vortex::Mesh *out){

    vortex::Timer timer;
    struct comp_vec{
        bool operator()(const vortex::Mesh::VertexData &lhv, const vortex::Mesh::VertexData &rhv) const {
            if( lhv.mVertex.x<rhv.mVertex.x || (lhv.mVertex.x==rhv.mVertex.x && lhv.mVertex.y<rhv.mVertex.y ) || (lhv.mVertex.x==rhv.mVertex.x && lhv.mVertex.y==rhv.mVertex.y && lhv.mVertex.z<rhv.mVertex.z )){
                return true;
            }
            else if( lhv.mNormal.x<rhv.mNormal .x || (lhv.mNormal .x==rhv.mNormal.x && lhv.mNormal.y<rhv.mNormal .y ) || (lhv.mNormal.x==rhv.mNormal .x && lhv.mNormal .y==rhv.mNormal .y && lhv.mNormal.z<rhv.mNormal .z )){
                return true;
            }
            else if( lhv.mTexCoord.x<rhv.mTexCoord.x || (lhv.mTexCoord.x==rhv.mTexCoord.x && lhv.mTexCoord.y<rhv.mTexCoord.y ) || (lhv.mTexCoord.x==rhv.mTexCoord.x && lhv.mTexCoord.y==rhv.mTexCoord.y && lhv.mTexCoord.z<rhv.mTexCoord.z )){
                return true;
            }
            return false;
        }
    };


    typedef std::map<vortex::Mesh::VertexData, int, comp_vec> vMap;
    vMap vertexHandles;

    timer.start();
    //in->triangulate();
    //in->garbage_collection();
    in->request_face_normals();
    in->update_face_normals();
    in->update_halfedge_normals(M_PI);
    in->release_face_normals();
    timer.stop();
    std::cout << "timer openmesh conversion " << timer.value() << std::endl;
    std::vector<int> meshIndices;
    std::vector<vortex::Mesh::VertexData> meshVertices;

    timer.reset();
    timer.start();
    // iterator over all faces
    unsigned int vertexIndex = 0;
    for (DefaultPolyMesh::FaceIter f_it=in->faces_begin(); f_it!=in->faces_end(); ++f_it){
        vortex::Mesh::VertexData v;
        int indices[3];
        int i=0;

        // iterator over vertex (thru haldedge to get access to halfedge normals)
        for(DefaultPolyMesh::FaceHalfedgeIter fv_it = in->fh_iter(*f_it); fv_it.is_valid(); ++fv_it){
            assert(i<3);
            DefaultPolyMesh::Point p = in->point(in->to_vertex_handle(*fv_it));
            DefaultPolyMesh::Normal n = in->normal(*fv_it);
            v.mVertex = glm::vec3(p[0], p[1], p[2]);
            v.mNormal = glm::vec3(n[0], n[1], n[2]);

            int vi;
            vMap::iterator vtr = vertexHandles.find(v);
            if(vtr == vertexHandles.end()){
                vi = vertexIndex ++;
                vertexHandles.insert( vtr, vMap::value_type(v, vi));
                meshVertices.push_back(v);
            }
            else{
                vi = vtr->second;
            }

            indices[i] = vi;
            i ++;
        }
        meshIndices.push_back(indices[0]);
        meshIndices.push_back(indices[1]);
        meshIndices.push_back(indices[2]);
    }
    assert(vertexIndex == meshVertices.size());

    timer.stop();
    std::cout << "timer parcours mesh " << timer.value() << std::endl;

    out->setData(out->name(), &(meshVertices[0]), meshVertices.size(), &(meshIndices[0]), meshIndices.size());
}

void MeshConverter::convert(vortex::Mesh *in, DefaultPolyMesh *out){
    out->request_halfedge_normals();

    struct comp_vec{
        bool operator()(const glm::vec3 &lhv, const glm::vec3 &rhv) const{
            return lhv.x<rhv.x || (lhv.x==rhv.x && lhv.y<rhv.y ) || (lhv.x==rhv.x && lhv.y==rhv.y && lhv.z<rhv.z );
        }
    };

    typedef std::map<glm::vec3, DefaultPolyMesh::VertexHandle, comp_vec> vMap;
    vMap vertexHandles;

    std::vector<DefaultPolyMesh::VertexHandle> face_vhandles;

    for(int i=0; i<in->numIndices(); i++)
    {
        glm::vec3 p = in->vertices()[in->indices()[i]].mVertex;
        using vortex::util::operator<<;
        vMap::iterator vtr = vertexHandles.find(p);
        DefaultPolyMesh::VertexHandle vh;
        if(vtr == vertexHandles.end())
        {
            vh = out->add_vertex( DefaultPolyMesh::Point(p.x, p.y, p.z));
            vertexHandles.insert( vtr, vMap::value_type(p, vh) );
        }
        else
        {
            vh = vtr->second;
        }
        face_vhandles.push_back(vh);
        if(((i+1)%3)==0)
        {
            DefaultPolyMesh::FaceHandle fh = out->add_face(face_vhandles);
            //!@warning fh halfedge points to the first vertex of vhandles, but it is not clearly specified
            DefaultPolyMesh::HalfedgeHandle hh = out->halfedge_handle(fh);
            glm::vec3 n;

            n = in->vertices()[in->indices()[i-2]].mNormal;
            out->set_normal(hh, DefaultPolyMesh::Normal(n.x, n.y, n.z));

            hh = out->next_halfedge_handle(hh);
            n = in->vertices()[in->indices()[i-1]].mNormal;
            out->set_normal(hh, DefaultPolyMesh::Normal(n.x, n.y, n.z));

            hh = out->next_halfedge_handle(hh);
            n = in->vertices()[in->indices()[i]].mNormal;
            out->set_normal(hh, DefaultPolyMesh::Normal(n.x, n.y, n.z));

            face_vhandles.clear();
        }
    }
}
