#ifndef MAPTOGRAPHER_H_
#define MAPTOGRAPHER_H_
// automatically generated single header file
#include <glm/glm.hpp>
#include <zlib/zlib.h>
#include <mini-archiver/MiniArchiver.h>
#include <functional>


//
// Included: Options.h
struct Options {	// 
	glm::ivec2 screen_size_;								// calculated
	glm::ivec2 font_size_ = glm::ivec2(16);		// font size
	glm::ivec2 canvas_size_ = glm::ivec2(40, 40);	// in glyph numbers
	glm::ivec2 canvas_size() {
		return canvas_size_ * font_size_;
	}
};	// 

//
// Included: DocumentOptionService.h
struct DocumentOptionService {
	glm::ivec2 size_ = glm::ivec2(10);

	// 3d
	bool use_3d_ = false;
	float z_distance_ = 16.0f;
	int base_layer_ = 3;
};

//
// Included: AppVector.h
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
	auto begin() const { return data_.begin(); }
	auto end() const { return data_.end(); }
	std::vector<T>	data_;
};

//
// Included: DocumentPalette.h
class DocumentPalette {
public:
	AppVector<glm::vec4> palette_;								// fix palette
	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		ar(mini_archiver::make_nvp("Palette", palette_));
	}
};

//
// Included: DocumentGlyph.h
using ColorPalette = AppVector<glm::vec4>;

#define SAFE_CALL(a)	if(a)	a

#define DEFAULT_GLYPH		-1
#define DEFAULT_BRUSH		-1


class DocumentGlyph {
public:
	DocumentGlyph() = default;

	short glyph_ = DEFAULT_GLYPH;
	short brush_ = DEFAULT_BRUSH;

	short front_palette_color_ = -1;
	short back_palette_color_ = -1;
	glm::vec4 color_ = glm::vec4(DEFAULT_GLYPH);
	glm::vec4 background_color_ = glm::vec4(0);	// alpha is 1 or 0, and back glyph is solid in case alpha is 1
	

	void SetBrush(short brush, short glyph, const glm::vec4& fore_color = glm::vec4(0), const glm::vec4& back_color = glm::vec4(0));
	void Set(short glyph, const glm::vec4& fore_color = glm::vec4(0), const glm::vec4& back_color = glm::vec4(0));
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
		}
	}

	bool SameAs(DocumentGlyph* g) {
		return (g && g->glyph_ == glyph_);// && g->background_color_ == background_color_ && g->color_ == color_);
	}

	bool BrushValid(unsigned brush_size) {
		return DEFAULT_BRUSH < brush_ && brush_ < (int)brush_size;
	}
protected:
};

//
// Included: Brush.h
class Brush : public DocumentGlyph {
public:
	Brush() {
	}
	std::string title_ = "Brush";

	std::vector<short> directional_glyph_;

	glm::vec4 fore_base_ = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);		// it will variate color by variations
	glm::vec4 back_base_ = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);		// it will variate color by variations

	glm::vec4 fore_variations_ = glm::vec4(0);		// it will variate color by variations
	glm::vec4 back_variations_ = glm::vec4(0);		// it will variate color by variations
	float	  animation_speed_ = 0.0f;			// it will animate if animation speed is > 0
	float	  percent_ = 0.0f;				// chance that percent 

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
		// force update of the glyphs on load/save
		recently_updated_ = true;

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

				//glyph->glyph_ = glyph_;

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
// Included: DocumentLayer.h
using BrushPalette = AppVector<Brush>;

template<class T>
class DocumentLayer {
protected:
	std::shared_ptr<DocumentOptionService>	doc_options_;	
public:	
	std::string			title_ = "New Layer";
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
	void UpdateBrushes(const BrushPalette* brush);

	
	T* GetAt(const glm::ivec2& pt);
	T* GetAt(int x, int y);
	T* GetAt(int n);

	T* GetAt(const glm::ivec2& pt) const;
	T* GetAt(int x, int y) const;
	T* GetAt(int n) const;

	void ChangeArraySize( unsigned to_size ); 
	void Resize(const glm::ivec2& new_size_);	// 
	glm::ivec2 Size() const {
		if (doc_options_ != nullptr)
			return doc_options_->size_;

		return glm::ivec2(1);
	}
};


//
// Included: DocumentRegion.h
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
		else if (ti == typeid(std::string))		type_ = Type_::CustomType_String;
		else if (ti == typeid(int))				type_ = Type_::CustomType_Int;
		else if (ti == typeid(bool))			type_ = Type_::CustomType_Bool;
		else if (ti == typeid(glm::vec2))		type_ = Type_::CustomType_Vec2;
		else if (ti == typeid(glm::vec3))		type_ = Type_::CustomType_Vec3;
		else if (ti == typeid(glm::vec4))		type_ = Type_::CustomType_Vec4;
		else if (ti == typeid(glm::ivec2))		type_ = Type_::CustomType_Vec2_Int;
	}

	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		ar(mini_archiver::make_nvp("Name", title_));

		int type = static_cast<unsigned char>(type_);
		ar(mini_archiver::make_nvp("Type", type));
		type_ = static_cast<Type_>(type);

		if (ar.IsLoader() == false ) {
			switch (type_) {
			case Type_::CustomType_Def:	break;
			case Type_::CustomType_String:		((VolatileObject<std::string>*)(this))->Serialize(ar, version);	 break;
			case Type_::CustomType_Int:			((VolatileObject<int>*)(this))->Serialize(ar, version);	break;
			case Type_::CustomType_Float:		((VolatileObject<float>*)(this))->Serialize(ar, version);	break;
			case Type_::CustomType_Bool:		((VolatileObject<bool>*)(this))->Serialize(ar, version);	break;
			case Type_::CustomType_Vec2:		((VolatileObject<glm::vec2>*)(this))->Serialize(ar, version);	break;
			case Type_::CustomType_Vec3:		((VolatileObject<glm::vec3>*)(this))->Serialize(ar, version);	break;
			case Type_::CustomType_Vec4:		((VolatileObject<glm::vec4>*)(this))->Serialize(ar, version);	break;
			case Type_::CustomType_Vec2_Int:	((VolatileObject<glm::ivec2>*)(this))->Serialize(ar, version);	break;
			}
		}
		
	}
	std::string title_;
	Type_ type_ = Type_::CustomType_Def;
};


template< typename  T>
class VolatileObject : public VolatileBaseObject
{
public:
	VolatileObject() = default;
	VolatileObject(T&& v) : value_(v) { VolatileBaseObject::Process<T>(v); }

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
	std::string		title_  = "";
	bool			visible_ = true;
	glm::ivec4		dimension_ = glm::ivec4(0);
	CustomDataArray custom_data_;

	void Init() {
		dimension_ = glm::ivec4(0);
		custom_data_.clear();
		title_ = "";
	}

	bool IsDefined() {
		return dimension_ != glm::ivec4(0);
	}


	template <class Archive>
	void Serialize(Archive& ar, unsigned version) {
		
		ar(mini_archiver::make_nvp("Name", title_));
		ar(mini_archiver::make_nvp("Visible", visible_));

		if( version >= 5)
			ar(mini_archiver::make_nvp("Dimension", dimension_));
		else if (version <= 4) {
			// TODO: remove me
			glm::vec4 dim; 
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
					AddNew<std::string>(vbo.title_, std::string(""));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<std::string>*)custom_data_.back().get())->value_));
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
					AddNew<glm::vec2>(vbo.title_, glm::vec2(0));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<glm::vec2>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Vec3:
					AddNew<glm::vec3>(vbo.title_, glm::vec3(0));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<glm::vec3>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Vec4:
					AddNew<glm::vec4>(vbo.title_, glm::vec4(0));	
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<glm::vec4>*)custom_data_.back().get())->value_));
					break;
				case VolatileBaseObject::Type_::CustomType_Vec2_Int:
					AddNew<glm::ivec2>(vbo.title_, glm::ivec2(0));
					ar(mini_archiver::make_nvp("Value", ((VolatileObject<glm::ivec2>*)custom_data_.back().get())->value_));
					break;
				}
			}
		}
		else {
			// saver
			ar(mini_archiver::make_nvp("CustomData", custom_data_));
		}
	}//*/

	void Clear() {
		custom_data_.clear();
	}
	template<typename T, typename... Args>
	void AddNew(const std::string& var_name, Args&&... args)
	{
		auto ptr = std::shared_ptr<VolatileBaseObject>(new VolatileObject<T>(std::forward<Args>(args)...));
		ptr->title_ = var_name;
		custom_data_.push_back( ptr);
	}

	VolatileBaseObject* AddByIndex(const std::string& var_name, int index) {
		switch (VolatileBaseObject::Type_(index)) {
		case VolatileBaseObject::Type_::CustomType_Float:			AddNew<float>(var_name, 0.0f);	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_String:			AddNew<std::string>(var_name, "");	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Int:				AddNew<int>(var_name, 0);	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Bool:			AddNew<bool>(var_name, true);	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec2:			AddNew<glm::vec2>(var_name, glm::vec2(0));	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec3:			AddNew<glm::vec3>(var_name, glm::vec3(0));	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec4:			AddNew<glm::vec4>(var_name, glm::vec4(0));	return custom_data_.back().get();
		case VolatileBaseObject::Type_::CustomType_Vec2_Int:		AddNew<glm::ivec2>(var_name, glm::ivec2(0));	return custom_data_.back().get();
		}

		return nullptr;
	}

	template<typename T>
	VolatileObject<T>* GetVolatileObject(const std::string& object_name, unsigned* id = nullptr) const {
		for (unsigned i = 0; i < custom_data_.size(); i++) {
			auto* cd = &custom_data_[i];

			if (cd && (*cd)->title_ == object_name) {
				if (id)
					*id = i;
				return ((VolatileObject<T>*)cd->get());
			}
		}

		return nullptr;
	}
};


//
// Included: BaseDocument.h
#define DEFAULT_DOC_TITLE		"untitled"

template<class T>
class BaseDocument {
protected:
	bool				dirty_ = false;							// document changed?
public:	// exposed member variables for ImGui
	std::shared_ptr<DocumentOptionService>	doc_options_;		
	std::string			title_			= DEFAULT_DOC_TITLE;	// document title
	bool				opened_			= true;					// visible in tab
	glm::ivec2			draw_offset_	= glm::ivec2(-1);		// document offset on canvas

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

		if( version >= 5 )
			ar(mini_archiver::make_nvp("Size", doc_options_->size_));
		else {
			glm::vec2 size;
			ar(mini_archiver::make_nvp("Size", size));
			doc_options_->size_.x = (int)size.x;
			doc_options_->size_.y = (int)size.y;
		}

		ar(mini_archiver::make_nvp("Use3D", doc_options_->use_3d_));
		ar(mini_archiver::make_nvp("ZDistance", doc_options_->z_distance_));
		ar(mini_archiver::make_nvp("BaseLayer", doc_options_->base_layer_));

		if (version >= 5)
			ar(mini_archiver::make_nvp("DrawOffset", draw_offset_));
		else
		{
			glm::vec2 size;
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

		Resize(doc_options_->size_);
	}

	DocumentRegion* GetRegion(const std::string& region_title, unsigned* id = nullptr) {
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
	bool Resize(const glm::ivec2& new_size_);			// 
};	// 

//
// Included: Document.h
class DocumentElement;		// 

template<class T>
class Document :
	public BaseDocument<T>,
	public DocumentPalette
{	
	binary_vector		key_;	// 
public:	// 
	Document() = default;
	virtual ~Document() = default;

	bool				has_own_palette_ = true;
	AppVector<DocumentElement>		elements_;					// smart objects and smaller document stuff
	AppVector<Brush>				brush_;

	virtual bool IsDocument() {
		return true;
	}

	template <class Archive>
	void Serialize(Archive & ar, unsigned version)
	{
		if (IsDocument()) {
			if (version >= 2) {
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
			ar(mini_archiver::make_nvp("Brushes", brush_));
		}
	}
	void SetKey(const binary_vector& key); // 
	void RefreshBrushes( const BrushPalette* brushes = nullptr);	// 
	bool Load(const std::string& path);	// 
	virtual void Clear() override;	// 
	// desc: gets element by name, retrieves eventual id
	DocumentElement* GetElement(const std::string& element_title, unsigned* id = nullptr) {
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
