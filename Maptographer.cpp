#include "Maptographer.h"
Options options_;

//
// Included: DocumentPalette.cpp

//
// Included: DocumentGlyph.cpp
void DocumentGlyph::Set(short glyph, const glm::vec4& fore_color, const glm::vec4& back_color)
{
	glyph_ = glyph;
	brush_ = DEFAULT_BRUSH;

	if (fore_color != glm::vec4(0)) {
		color_ = fore_color;
		front_palette_color_ = -1;
	}

	if (back_color != glm::vec4(0)) {
		background_color_ = back_color;
		back_palette_color_ = -1;
	}
}

void DocumentGlyph::Set(short glyph, int fore_index /*= 0*/, int back_index /*= 0*/)
{
	glyph_ = glyph;
	brush_ = DEFAULT_BRUSH;

	if (fore_index != -1)
		front_palette_color_ = fore_index;

	if (back_index != -1)
		back_palette_color_ = back_index;
}

bool DocumentGlyph::Default() const
{
	return glyph_ == DEFAULT_GLYPH && brush_ == DEFAULT_BRUSH;
}

void DocumentGlyph::SetBrush(short brush, short glyph, const glm::vec4& fore_color /*= glm::vec4(0)*/, const glm::vec4& back_color /*= glm::vec4(0)*/)
{
	glyph_ = glyph;
	brush_ = brush;
	if (fore_color != glm::vec4(0)) {
		color_ = fore_color;
		front_palette_color_ = -1;
	}

	if (back_color != glm::vec4(0)) {
		background_color_ = back_color;
		back_palette_color_ = -1;
	}
}
void DocumentGlyph::Clear()
{
	glyph_ = DEFAULT_GLYPH;
	brush_ = DEFAULT_BRUSH;
	front_palette_color_ = -1;
	back_palette_color_ = -1;
	color_ = glm::vec4(DEFAULT_GLYPH);
	background_color_ = glm::vec4(0);
}


//
// Included: Brush.cpp

//
// Included: DocumentLayer.cpp
template class DocumentLayer<DocumentGlyph>;	// 
template<class T>
void DocumentLayer<T>::UpdateBrushes(const BrushPalette* brush)
{
	if (brush) {
		for (auto& t : data_) {
			if (t.BrushValid(brush->Size())) {
				const Brush* brush_piece = const_cast<BrushPalette*>(brush)->Get(t.brush_);
				if (brush_piece->NeedRefresh()) {
					brush_piece->UpdateForeground(&t, true);
					brush_piece->UpdateBackground(&t, true);
				}
			}
		}
	}
}
template<class T>
void DocumentLayer<T>::Resize(const glm::ivec2& new_size_)
{
	auto size_ = Size();
	if (new_size_ != size_ || (data_.size() != new_size_.x*new_size_.y)) {
		auto backup = data_;
		auto old_size_ = size_;

		ChangeArraySize((unsigned)(new_size_.x*new_size_.y));

		doc_options_->size_ = new_size_;

		for (int y = 0; y < old_size_.y; y++) {
			for (int x = 0; x < old_size_.x; x++) {
				int index = x + y * (int)old_size_.x;
				if (index >= (int)backup.size())
					continue;

				auto* old_tile = &backup[index];
				auto* new_tile = GetAt(glm::ivec2(x, y));

				if (new_tile && old_tile )
					*new_tile = *old_tile;
			}
		}

		doc_options_->size_ = old_size_;
	}
}

template<class T>
T * DocumentLayer<T>::GetAt(const glm::ivec2 & pt)
{
	return GetAt(pt.x, pt.y);
}

template<class T>
T* DocumentLayer<T>::GetAt(int x, int y)
{
	auto size_ = Size();
	if (x >= 0 && x < (int)size_.x && y >= 0 && y < (int)size_.y)
		return &data_[x + y * (int)size_.x];

	return nullptr;
}

template<class T>
T * DocumentLayer<T>::GetAt(int n)
{
	auto size_ = Size();
	if (n >= 0 && n < (int)(size_.x*size_.y))
		return &data_[n];
	return nullptr;
}

template<class T>
T * DocumentLayer<T>::GetAt(const glm::ivec2 & pt) const
{
	return GetAt(pt.x, pt.y);
}

template<class T>
T* DocumentLayer<T>::GetAt(int x, int y) const
{
	auto size_ = Size();
	if (x >= 0 && x < (int)size_.x && y >= 0 && y < (int)size_.y)
		return const_cast<T*>(&data_[x + y * (int)size_.x]);

	return nullptr;
}

template<class T>
T * DocumentLayer<T>::GetAt(int n) const
{
	auto size_ = Size();
	if (n >= 0 && n < (int)(size_.x*size_.y))
		return const_cast<T*>(&data_[n]);
	return nullptr;
}

template<class T>
void DocumentLayer<T>::ChangeArraySize(unsigned to_size)
{
	data_.clear();
	for (unsigned i = 0; i < to_size; i++)
		data_.push_back(T());
}

//
// Included: DocumentRegion.cpp
DocumentRegion::DocumentRegion()
{
}


//
// Included: BaseDocument.cpp
template class BaseDocument<DocumentGlyph>; // 
// desc: clears document
template<class T>
void BaseDocument<T>::Clear() {
	layer_.Clear();
	draw_offset_ = glm::vec2(-1);

}
template<class T>
bool BaseDocument<T>::Resize(const glm::ivec2& new_size_)
{
	if (new_size_.x > 0 && new_size_.y > 0) {
		layer_.Call(&DocumentLayer<T>::Resize, new_size_);


		doc_options_->size_ = new_size_;

		return true;
	}

	return false;
}

//
// Included: Document.cpp
template class Document<DocumentGlyph>;	// 
template<class T>
void Document<T>::RefreshBrushes(const BrushPalette* brushes) {

	if (brushes == nullptr) {

		brush_.Call(&Brush::RefreshSelf);
		brush_.Call(&Brush::ForceRefresh);
		
		RefreshBrushes(&brush_);

		elements_.Call(&DocumentElement::RefreshBrushes, &brush_);
	}
	else {
		BaseDocument<T>::layer_.Call(&DocumentLayer<T>::UpdateBrushes, brushes);
	}
}

template<class T>
bool Document<T>::Load(const std::string & path)
{
	Clear();
	BinaryIn file(path);

	if (file.is_open() == false)
		return false;

	if (file.get_stream_type() & STREAM_TYPE_COMPRESSED) {
		file.read_compressed_data();
	}

	file(*this);
	file.close();
	RefreshBrushes();
	return true;
} 
template<class T>
void Document<T>::Clear()
{
	BaseDocument<T>::Clear();
	elements_.Clear();
}
