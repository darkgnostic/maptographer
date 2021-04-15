#ifndef MAPTOGRAPHER_H_
#define MAPTOGRAPHER_H_
// automatically generated single header file
#include <glm/glm.hpp>
#include <zlib.h>
#include <mini-archiver/MiniArchiver.h>
#include <functional>
#include <memory>


//
// Included: ../../Maptographer/src/Config.h
#ifdef NEW_ONE
#include "engine/type/Vector2D.h"
#include "engine/type/Vector3D.h"
#include "engine/type/Vector4D.h"

using mpt_vec2i = nox::vec2di;
using mpt_vec2f = nox::vec2df;
using mpt_vec3f = nox::Vector3d<float>;
using mpt_vec4f = nox::vec4df;
using mpt_vec4i = nox::Vector4d<int>;
using mpt_string = nostd::string;
#else
using mpt_vec2i = glm::ivec2;
using mpt_vec2f = glm::vec2;
using mpt_vec3f = glm::vec3;
using mpt_vec4f = glm::vec4;
using mpt_vec4i = glm::ivec4;
using mpt_string = std::string;
#endif

//
// Included: ../../Maptographer/src/ToolType.h

//
// Included: ../../Maptographer/src/Options.h
struct Options {	// 
	mpt_vec2i screen_size_;								// calculated
	mpt_vec2i font_size_ = mpt_vec2i(16);		// font size
	mpt_vec2i canvas_size_ = mpt_vec2i(40, 40);	// in glyph numbers
	mpt_vec2i canvas_size_in_px() {
		return canvas_size_ * font_size_;
	}
};	// 

//
// Included: ../../Maptographer/src/DocumentOptionService.h
struct DocumentOptionService {
	// 3d
	bool use_3d_ = false;
	float z_distance_ = 16.0f;
	int base_layer_ = 3;
};

//
// Included: ../../Maptographer/src/AppVector.h
// simple std::vector wrapper
template<class T>
class AppVector {
public:
	AppVector() = default;
	virtual ~AppVector() {
	}
	
	template <typename V>
	bool ReturnCall(V f) {
		for (auto& elem : data_)
			if ((elem.*f)())
				return true;

		return false;
	}
	template <typename V>
	void Call(V f) {
		for (auto& elem : data_)
			(elem.*f)();
	}
	template <typename V, typename Param>
	void Call(V f, Param p) {
		for (auto& elem : data_)
			(elem.*f)(p);
	}

	T* New() {
		data_.push_back(T());
		return &data_.back();
	}

	T* New(T& data) {
		data_.push_back(data);
		return &data_.back();
	}
	T* New(T&& data) {
		data_.push_back(data);
		return &data_.back();
	}
	
	T* Duplicate(unsigned id) {
		if (IdValid(id)) {
			auto element = data_[id];
			data_.insert(data_.begin() + id + 1, 1, element);
			return &data_[id + 1];
		}
		return nullptr;
	}
	bool Delete(unsigned id) {
		if (IdValid(id)) {
			data_.erase(data_.begin() + id);
			return true;
		}

		return false;
	}

	T* Get(unsigned id) {
		if (IdValid(id))
			return &data_[id];

		return nullptr;
	}

	void Set(unsigned id, const T& data) {
		if (IdValid(id))
			data_[id] = data;
	}

	bool IdValid(unsigned id) const {
		return id < data_.size();;
	}

	size_t Size() const {
		return  data_.size();
	}

	void Swap(unsigned id_1, unsigned id_2) {
		if (IdValid(id_1) && IdValid(id_2))
			std::swap(data_[id_1], data_[id_2]);
	}
	bool Move(unsigned id_1, unsigned id_2) {
		if (IdValid(id_1) && IdValid(id_2)) {

			if (id_1 > id_2) {
				data_.insert(data_.begin() + id_1 + 1, 1, data_[id_2]);
				Delete(id_2);
				return true;
			}

			if (id_1 < id_2) {
				data_.insert(data_.begin() + id_1, 1, data_[id_2]);
				Delete(id_2 + 1);
				return true;
			}
		}

		return false;
	}

	void Clear() {
		data_.clear();
	}
	
	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		ar(mini_archiver::make_nvp("data", data_));
	}
	auto begin()  { return data_.begin(); }
	auto end()  { return data_.end(); }
	std::vector<T>	data_;
};

//
// Included: ../../Maptographer/src/DocumentPalette.h
class DocumentPalette {
public:
	AppVector<mpt_vec4f> palette_;								// fix palette
	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		ar(mini_archiver::make_nvp("Palette", palette_));
	}
};

//
// Included: ../../Maptographer/src/DocumentGlyph.h
using ColorPalette = AppVector<mpt_vec4f>;

#define SAFE_CALL(a)	if(a)	a

#define DEFAULT_GLYPH		-1
#define DEFAULT_BRUSH		-1
#define DEFAULT_COLOR		-1


class DocumentGlyph {
public:
	DocumentGlyph() = default;

	short glyph_ = DEFAULT_GLYPH;
	short brush_ = DEFAULT_BRUSH;

	short front_palette_color_ = -1;
	short back_palette_color_ = -1;
	mpt_vec4f color_ = mpt_vec4f(DEFAULT_GLYPH);
	mpt_vec4f background_color_ = mpt_vec4f(0);	// alpha is 1 or 0, and back glyph is solid in case alpha is 1
	

	void SetBrush(short brush, short glyph, const mpt_vec4f& fore_color = mpt_vec4f(0), const mpt_vec4f& back_color = mpt_vec4f(0));
	void Set(short glyph, const mpt_vec4f& fore_color = mpt_vec4f(0), const mpt_vec4f& back_color = mpt_vec4f(0));
	void Set(short glyph, int fore_index = 0, int back_index = 0);

	void Clear();
	bool Default() const;

	template <class Archive>
	void Serialize(Archive& ar, unsigned version) {

		ar(mini_archiver::make_nvp("Brush", brush_));
		ar(mini_archiver::make_nvp("Glyph", glyph_));
		if (glyph_ != DEFAULT_GLYPH) {
			ar(
				mini_archiver::make_nvp("FrontPaletteIndex", front_palette_color_),
				mini_archiver::make_nvp("BackPaletteIndex", back_palette_color_)
			);

			if( version >= 7 )
				ar(color_, background_color_);
		}
	}

	bool SameAs(DocumentGlyph* g) {
		return (g && g->glyph_ == glyph_);// && g->background_color_ == background_color_ && g->color_ == color_);
	}

	bool BrushValid() const	{
		return brush_ != DEFAULT_BRUSH;
	}

	bool BrushValid(unsigned brush_size) {
		return DEFAULT_BRUSH < brush_ && brush_ < (int)brush_size;
	}
protected:
};

//
// Included: ../../Maptographer/src/Brush.h
//
//
//#include <bitset>
#include <array>

#define MAX_BRUSH_ENTRIES_PER_SET		8192
class Brush : public DocumentGlyph {
public:
	Brush() {
	}
	mpt_string title_ = "Brush";

	std::vector<short> directional_glyph_;

	mpt_vec4f fore_base_ = mpt_vec4f(0.5f, 0.5f, 0.5f, 1.0f);		// it will variate color by variations
	mpt_vec4f back_base_ = mpt_vec4f(0.0f, 0.0f, 0.0f, 1.0f);		// it will variate color by variations

	mpt_vec4f fore_variations_ = mpt_vec4f(0);		// it will variate color by variations
	mpt_vec4f back_variations_ = mpt_vec4f(0);		// it will variate color by variations
	float	  animation_speed_ = 0.0f;			// it will animate if animation speed is > 0
	float	  percent_ = 0.0f;				// chance that percent

	std::vector<unsigned char> flags_;

	// no need to serialize
	float last_check_ = 0.0f;
	bool recently_updated_ = false;
	bool force_refresh_ = false;
	bool connected_ = false;

	template <class Archive>
	void Serialize(Archive & archive, unsigned version)
	{
		archive(mini_archiver::make_nvp("Title", title_));
		archive(mini_archiver::make_nvp("Glyph", glyph_));

		archive(
			mini_archiver::make_nvp("ForegroundBase", fore_base_),
			mini_archiver::make_nvp("BackgroundBase", back_base_),
			mini_archiver::make_nvp("ForegroundVar", fore_variations_),
			mini_archiver::make_nvp("BackgroundVar", back_variations_),
			mini_archiver::make_nvp("AnimationSpeed", animation_speed_),
			mini_archiver::make_nvp("TriggerPercent", percent_)
		); // seria

		archive(mini_archiver::make_nvp("Connected", connected_));

		if( version > 8 ) {
			archive(mini_archiver::make_nvp("Flags", flags_));
		}
		// force update of the glyphs on load/save
		recently_updated_ = true;

		ForceRefresh();
		RefreshSelf();

		last_check_ = 0.0f;

	}

	void AddFlag()
	{
		flags_.emplace_back(0)	;
	}

	void RemoveFlag()
	{
		flags_.pop_back();
	}

	bool NeedRefresh() const {
		return force_refresh_ || recently_updated_;
	}

	void ForceRefresh() {
		force_refresh_ = true;
		recently_updated_ = true;
	}

	void RefreshSelf() {
		UpdateForeground(this, true);
		UpdateBackground(this, true);
		recently_updated_ = true;
	}

	bool Update(float time) {
		if (animation_speed_ > 0.0f) {

			last_check_ += time;
			if (last_check_ >= animation_speed_) {
				last_check_ -= animation_speed_;
				recently_updated_ = true;

				// this is editor stuff, it will update flashing glyphs in tool window
				UpdateForeground(this);
				UpdateBackground(this);
				return true;
			}
		}

		if (force_refresh_ && recently_updated_) {
			force_refresh_ = false;
		}
		else {
			recently_updated_ = false;
		}

		return false;
	}

#define NV(a) a = a < -1 ? -1 : (a > 1 ? 1 : a);

	bool UpdateForeground(DocumentGlyph* glyph, bool force_update = false ) const {
		if ((recently_updated_ || force_update) && glyph_) {
			if (RandomPercent()) {

				// update glyph in case it is refreshed
				glyph->glyph_ = glyph_;

				glyph->color_.r = fore_base_.r + fore_variations_.r * (std::rand() / (float)RAND_MAX);
				glyph->color_.g = fore_base_.g + fore_variations_.g * (std::rand() / (float)RAND_MAX);
				glyph->color_.b = fore_base_.b + fore_variations_.b * (std::rand() / (float)RAND_MAX);
				glyph->color_.a = fore_base_.a + fore_variations_.a * (std::rand() / (float)RAND_MAX);

				NV(glyph->color_.r);
				NV(glyph->color_.g);
				NV(glyph->color_.b);
				NV(glyph->color_.a);

				return true;
			}
		}

		return false;
	}

	bool UpdateBackground(DocumentGlyph* glyph, bool force_update = false) const {
		if ((recently_updated_ || force_update) && glyph_) {
			if (RandomPercent()) {

				//glyph->glyph_ = glyph_;

				glyph->background_color_.r = back_base_.r + back_variations_.r * (std::rand() / (float)RAND_MAX);
				glyph->background_color_.g = back_base_.g + back_variations_.g * (std::rand() / (float)RAND_MAX);
				glyph->background_color_.b = back_base_.b + back_variations_.b * (std::rand() / (float)RAND_MAX);
				glyph->background_color_.a = back_base_.a + back_variations_.a * (std::rand() / (float)RAND_MAX);

				NV(glyph->background_color_.r);
				NV(glyph->background_color_.g);
				NV(glyph->background_color_.b);
				NV(glyph->background_color_.a);

				return true;
			}
		}

		return false;
	}

	bool RandomPercent() const {

		if (animation_speed_ == 0.0f || force_refresh_ || percent_ == 100.0f)
			return true;

		if (percent_ == 0.0f)
			return false;

		return std::rand() % 100 <= percent_;
	}
};


//
// Included: ../../Maptographer/src/DocumentLayer.h
const int kBrushFlagSize = 8;
class BrushPalette  : public AppVector<Brush>
{
public:
	mpt_string name_;

	std::vector<mpt_string> flag_name_;
	
	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		AppVector<Brush>::Serialize(ar,version);
		
		if( version > 8 ) {
			ar(mini_archiver::make_nvp("Name", name_));
			ar(mini_archiver::make_nvp("FlagNames", flag_name_));
			if( name_ .empty())
				name_ = "Brush Palette";
		}
	}

	void AddFlag()
	{
		for( int i = 0; i<kBrushFlagSize;i++ )
			flag_name_.emplace_back("");
		
		Call(&Brush::AddFlag);
	}

	void RemoveFlag()
	{
		for( int i = 0; i<kBrushFlagSize;i++ )
			flag_name_.pop_back();
		
		Call(&Brush::RemoveFlag);
	}
	
	Brush* Find(const mpt_string& name )
	{
		for( auto i =0; i< Size(); i++) {
			auto* brush = Get(i);
			if( brush &&  brush->title_== name ) {
				return brush;
			}
		}
	
		return nullptr;
	}
};
using BrushSet		= AppVector<BrushPalette>;	// 

template<class T>
class DocumentLayer {
protected:
	std::shared_ptr<DocumentOptionService>	doc_options_;	
public:	
	mpt_vec2i			size_ = mpt_vec2i(10);
	mpt_string			title_ = "New Layer";
	bool				visible_ = true;
	bool				locked_ = false;
	float				opacity_ = 1.0f;


	std::vector<T>	data_;

	DocumentLayer() {
	}

	DocumentLayer(std::shared_ptr<DocumentOptionService> doc_options) : doc_options_(doc_options) {
	}

	void SetOptionsService(std::shared_ptr<DocumentOptionService>	doc_options) {
		doc_options_ = doc_options;
	}

	template <class Archive>
	void Serialize(Archive& archive, unsigned version) 
	{
		archive(
			mini_archiver::make_nvp("Title", title_),
			mini_archiver::make_nvp("Visible", visible_),
			mini_archiver::make_nvp("Locked", locked_),
			mini_archiver::make_nvp("Opacity", opacity_)
		);
		archive(mini_archiver::make_nvp("Data", data_));

	}
	void SetSize(const mpt_vec2i size) {
		size_ = size;
	}
	void UpdateBrushes(const BrushPalette* brush);

	
	T* GetAt(const mpt_vec2i& pt);
	T* GetAt(int x, int y);
	T* GetAt(int n);

	T* GetAt(const mpt_vec2i& pt) const;
	T* GetAt(int x, int y) const;
	T* GetAt(int n) const;

	void ChangeArraySize( unsigned to_size ); 
	void Resize(const mpt_vec2i& new_size_);	// 
	mpt_vec2i Size() const {
		return size_;
	}
};


//
// Included: ../../Maptographer/src/DocumentRegion.h
template< typename  T>
class VolatileObject;

class VolatileBaseObject {
public:
	VolatileBaseObject() = default;
	virtual ~VolatileBaseObject() = default;
	enum class Type_ : unsigned char {
		CustomType_Def = 0,
		CustomType_String = 1,
		CustomType_Int = 2,
		CustomType_Float = 3,
		CustomType_Bool = 4,
		CustomType_Vec2 = 5,
		CustomType_Vec3 = 6,
		CustomType_Vec4 = 7,
		CustomType_Vec2_Int = 8,
	};
	template<typename T>
	void Process(T& v) {


		const std::type_info& ti = typeid(v);

		if (ti == typeid(float))				type_ = Type_::CustomType_Float;
		else if (ti == typeid(mpt_string))		type_ = Type_::CustomType_String;
		else if (ti == typeid(int))				type_ = Type_::CustomType_Int;
		else if (ti == typeid(bool))			type_ = Type_::CustomType_Bool;
		else if (ti == typeid(mpt_vec2f))		type_ = Type_::CustomType_Vec2;
		else if (ti == typeid(mpt_vec3f))		type_ = Type_::CustomType_Vec3;
		else if (ti == typeid(mpt_vec4f))		type_ = Type_::CustomType_Vec4;
		else if (ti == typeid(mpt_vec2i))		type_ = Type_::CustomType_Vec2_Int;
	}

	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		ar(mini_archiver::make_nvp("Name", title_));

		int type = static_cast<unsigned char>(type_);
		ar(mini_archiver::make_nvp("Type", type));
		type_ = static_cast<Type_>(type);

		if (ar.IsLoader() == false ) {
		    /* TODO: M1
			switch (type_) {
			case Type_::CustomType_Def:	break;
			case Type_::CustomType_String:		reinterpret_cast<VolatileObject<mpt_string>*>(this)->Serialize(ar, version);	 break;
			case Type_::CustomType_Int:			reinterpret_cast<VolatileObject<int>*>(this)->Serialize(ar, version);	break;
			case Type_::CustomType_Float:		reinterpret_cast<VolatileObject<float>*>(this)->Serialize(ar, version);	break;
			case Type_::CustomType_Bool:		reinterpret_cast<VolatileObject<bool>*>(this)->Serialize(ar, version);	break;
			case Type_::CustomType_Vec2:		reinterpret_cast<VolatileObject<mpt_vec2f>*>(this)->Serialize(ar, version);	break;
			case Type_::CustomType_Vec3:		reinterpret_cast<VolatileObject<mpt_vec3f>*>(this)->Serialize(ar, version);	break;
			case Type_::CustomType_Vec4:		reinterpret_cast<VolatileObject<mpt_vec4f>*>(this)->Serialize(ar, version);	break;
			case Type_::CustomType_Vec2_Int:	reinterpret_cast<VolatileObject<mpt_vec2i>*>(this)->Serialize(ar, version);	break;
			}*/
		}
		
	}
	mpt_string title_;
	Type_ type_ = Type_::CustomType_Def;
};


template< typename  T>
class VolatileObject final : public VolatileBaseObject
{
public:
	VolatileObject() = default;
	explicit VolatileObject(T&& v) : value_(v) { VolatileBaseObject::Process<T>(v); }

	T GetValue() { return value_; }
	T value_;

	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		//VolatileBaseObject::Serialize(ar, version);
		ar(mini_archiver::make_nvp("Value", value_));
	}
};

using CustomDataArray = std::vector< std::shared_ptr<VolatileBaseObject> >;


class DocumentRegion {
public:
	DocumentRegion();
	mpt_string		title_  = "";
	bool			visible_ = true;
	mpt_vec4i		dimension_ = mpt_vec4i(0);
	CustomDataArray custom_data_;

	void Init() {
		dimension_ = mpt_vec4i(0);
		custom_data_.clear();
		title_ = "";
	}

	bool IsDefined() {
		return dimension_ != mpt_vec4i(0);
	}


	template <class Archive>
	void Serialize(Archive& ar, unsigned version) {
		
		ar(mini_archiver::make_nvp("Name", title_));
		ar(mini_archiver::make_nvp("Visible", visible_));

		if( version >= 5)
			ar(mini_archiver::make_nvp("Dimension", dimension_));
		else if (version <= 4) {
			// TODO: remove me
			mpt_vec4f dim; 
			ar(mini_archiver::make_nvp("Dimension", dim));
			dimension_.x = (int)dim.x;
			dimension_.y = (int)dim.y;
			dimension_.z = (int)dim.z;
			dimension_.w = (int)dim.w;
		}

		if (ar.IsLoader()) {
			unsigned size = 0;
			ar(mini_archiver::make_nvp("Size", size));

			for (unsigned int i = 0; i < size; i++) {
				VolatileBaseObject vbo;
				
				ar(mini_archiver::make_nvp("Vbo", vbo));
				switch (vbo.type_) {
				case VolatileBaseObject::Type_::CustomType_Def:	break;
				case VolatileBaseObject::Type_::CustomType_String:
					AddNew<mpt_string>(vbo.title_, mpt_string(""));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_string>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Int:
					AddNew<int>(vbo.title_, 0);	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<int>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Float:
					AddNew<float>(vbo.title_, 0.0f);	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<float>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Bool:
					AddNew<bool>(vbo.title_, true);	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<bool>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Vec2:
					AddNew<mpt_vec2f>(vbo.title_, mpt_vec2f(0));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_vec2f>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Vec3:
					AddNew<mpt_vec3f>(vbo.title_, mpt_vec3f(0));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_vec3f>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Vec4:
					AddNew<mpt_vec4f>(vbo.title_, mpt_vec4f(0));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_vec4f>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Vec2_Int:
					AddNew<mpt_vec2i>(vbo.title_, mpt_vec2i(0));
					ar(mini_archiver::make_nvp("Value", reinterpret_cast<VolatileObject<mpt_vec2i>*>(custom_data_.back().get())->value_));
					break;
				}
			}
		}
		else {
			// saver
			//ar(mini_archiver::make_nvp("CustomData", custom_data_));
            unsigned size = custom_data_.size();
            ar(mini_archiver::make_nvp("Size", size));

            for (unsigned int i = 0; i < size; i++) {
                ar(mini_archiver::make_nvp("Vbo", custom_data_[i]));
                auto* cd = custom_data_[i].get();
                switch (custom_data_[i]->type_) {
                    case VolatileBaseObject::Type_::CustomType_Def:	break;
                    case VolatileBaseObject::Type_::CustomType_String:
                        ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_string>*)cd)->value_));
                        break;
                    case VolatileBaseObject::Type_::CustomType_Int:
                        ar(mini_archiver::make_nvp("Value", ((VolatileObject<int>*)cd)->value_));
                        break;
                    case VolatileBaseObject::Type_::CustomType_Float:
                        ar(mini_archiver::make_nvp("Value", ((VolatileObject<float>*)cd)->value_));
                        break;
                    case VolatileBaseObject::Type_::CustomType_Bool:
                        ar(mini_archiver::make_nvp("Value", ((VolatileObject<bool>*)cd)->value_));
                        break;
                    case VolatileBaseObject::Type_::CustomType_Vec2:
                        ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_vec2f>*)cd)->value_));
                        break;
                    case VolatileBaseObject::Type_::CustomType_Vec3:
                        ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_vec3f>*)cd)->value_));
                        break;
                    case VolatileBaseObject::Type_::CustomType_Vec4:
                        ar(mini_archiver::make_nvp("Value", ((VolatileObject<mpt_vec4f>*)cd)->value_));
                        break;
                    case VolatileBaseObject::Type_::CustomType_Vec2_Int:
                        ar(mini_archiver::make_nvp("Value", reinterpret_cast<VolatileObject<mpt_vec2i>*>(cd)->value_));
                        break;
                }
            }
		}
	}//*/

	void Clear() {
		custom_data_.clear();
	}
	template<typename T, typename... Args>
	void AddNew(const mpt_string& var_name, Args&&... args)
	{
		auto ptr = std::shared_ptr<VolatileBaseObject>(new VolatileObject<T>(std::forward<Args>(args)...));
		ptr->title_ = var_name;
		custom_data_.push_back( ptr);
	}

	VolatileBaseObject* AddByIndex(const mpt_string& var_name, int index) {
		switch (VolatileBaseObject::Type_(index)) {
		case VolatileBaseObject::Type_::CustomType_Float:			AddNew<float>(var_name, 0.0f);	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_String:			AddNew<mpt_string>(var_name, "");	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Int:				AddNew<int>(var_name, 0);	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Bool:			AddNew<bool>(var_name, true);	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec2:			AddNew<mpt_vec2f>(var_name, mpt_vec2f(0));	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec3:			AddNew<mpt_vec3f>(var_name, mpt_vec3f(0));	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec4:			AddNew<mpt_vec4f>(var_name, mpt_vec4f(0));	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec2_Int:		AddNew<mpt_vec2i>(var_name, mpt_vec2i(0));	return custom_data_.back().get();
		default:
			break;
		}

		return nullptr;
	}

	template<typename T>
	VolatileObject<T>* GetVolatileObject(const mpt_string& object_name, unsigned* id = nullptr) const {
		for (unsigned i = 0; i < custom_data_.size(); i++) {
			auto* cd = &custom_data_[i];

			if (cd && (*cd)->title_ == object_name) {
				if (id)
					*id = i;
				return static_cast<VolatileObject<T>*>(cd->get());
			}
		}

		return nullptr;
	}
};


//
// Included: ../../Maptographer/src/BaseDocument.h
#define DEFAULT_DOC_TITLE		"untitled"

template<class T>
class BaseDocument {
protected:
	bool				dirty_ = false;							// document changed?
public:	// exposed member variables for ImGui
	std::shared_ptr<DocumentOptionService>	doc_options_;		
	mpt_vec2i			size_ = mpt_vec2i(10);
	mpt_string			title_			= DEFAULT_DOC_TITLE;	// document title
	bool				opened_			= true;					// visible in tab
	mpt_vec2i			draw_offset_	= mpt_vec2i(-1);		// document offset on canvas

																// layer is copied to document layers when appropriate
	AppVector<DocumentLayer<T>>	layer_;							// layers
	
	bool				using_palette_ = true;					// are we suing palette or true color
	AppVector<DocumentRegion>	regions_;
	//std::vector<Brush>			brush_;
public:

	BaseDocument() {
	}

	virtual ~BaseDocument() = default;
	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		if (doc_options_ == nullptr) {
			doc_options_ = std::make_shared<DocumentOptionService>();
		}

		ar(mini_archiver::make_nvp("Title", title_));
		ar(mini_archiver::make_nvp("Size", size_));
		ar(mini_archiver::make_nvp("Use3D", doc_options_->use_3d_));
		ar(mini_archiver::make_nvp("ZDistance", doc_options_->z_distance_));
		ar(mini_archiver::make_nvp("BaseLayer", doc_options_->base_layer_));

		if (version >= 5)
			ar(mini_archiver::make_nvp("DrawOffset", draw_offset_));
		else
		{
			mpt_vec2f size;
			ar(mini_archiver::make_nvp("DrawOffset", size));
			draw_offset_.x = (int)size.x;
			draw_offset_.y = (int)size.y;
		}
		ar(mini_archiver::make_nvp("UsingPalette", using_palette_));

		ar(mini_archiver::make_nvp("Layer", layer_));

		ar(mini_archiver::make_nvp("Regions", regions_));

		if (version >= 4)
			ar(mini_archiver::make_nvp("Opened", opened_));


		// on load (and as miseffect save) resize copy/dirty layer


		layer_.Call(&DocumentLayer<T>::SetOptionsService, doc_options_);
		layer_.Call(&DocumentLayer<T>::SetSize, size_);

		Resize(size_);
	}

	DocumentRegion* GetRegion(const mpt_string& region_title, unsigned* id = nullptr) {
		for (unsigned i = 0; i < regions_.Size(); i++) {
			if (auto* e = regions_.Get(i)) {
				if (e->title_ == region_title) {
					if (id)
						*id = i;
					return e;
				}
			}
		}

		return nullptr;
	}
	virtual void Clear();	// 
	virtual bool IsDocument() {
	    return true;
	}
	bool Resize(const mpt_vec2i& new_size_);			// 
};	// 

//
// Included: ../../Maptographer/src/Document.h
class DocumentElement;		// 

template<class T>
class Document :
	public BaseDocument<T>,
	public DocumentPalette
{	
	binary_vector		key_;	// 
	int					current_brush_palette_ = 0;	// 
public:	// 
	Document() = default;
	virtual ~Document() = default;

	bool							has_own_palette_ = true;
	AppVector<DocumentElement>		elements_;					// smart objects and smaller document stuff
	BrushSet						brush_;

	bool IsDocument() override {
		return true;
	}

	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		if (IsDocument()) {
			if (version >= 2 && version < 10 ) {
				bool compressed = false;
				ar(mini_archiver::make_nvp("Compressed", compressed));
			}
		}
		BaseDocument<T>::Serialize(ar, version);

		ar(mini_archiver::make_nvp("HasOwnPalette", has_own_palette_));

		if (has_own_palette_)
			DocumentPalette::Serialize(ar, version);

		if (IsDocument()) {
			ar(mini_archiver::make_nvp("Elements", elements_));

			if( version > 7 ) {
				ar(mini_archiver::make_nvp("Brushes", brush_));
			}
			else {
				brush_.Clear();
				auto* palette = brush_.New();
				ar(mini_archiver::make_nvp("Brushes", *palette));
			}
		}
	}
	void SetKey(const binary_vector& key); // 
	void RefreshBrushes( const BrushPalette* brushes = nullptr);	// 
	void RebuildBrushes();	// 
	LoadState Load(const mpt_string& path);	// 
	void Clear() override;	// 
	BrushPalette* CurrentBrushPalette();	// 
	// desc: gets element by name, retrieves eventual id
	DocumentElement* GetElement(const mpt_string& element_title, unsigned* id = nullptr) {
		for (unsigned i = 0; i < elements_.Size(); i++) {
			if (auto* e = elements_.Get(i)) {
				if (e->title_ == element_title) {
					if (id)
						*id = i;
					return e;
				}
			}
		}

		return nullptr;
	}

};	// 
class DocumentElement : public Document<DocumentGlyph> {
public:
	DocumentElement() {
		has_own_palette_ = false;
	}

	virtual bool IsDocument() override { return false; }

};
#endif
