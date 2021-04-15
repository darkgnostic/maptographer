#include "Maptographer.h"
Options options_;

//
// Included: ../../Maptographer/src/DocumentPalette.cpp

//
// Included: ../../Maptographer/src/DocumentGlyph.cpp
void DocumentGlyph::Set(short glyph, const mpt_vec4f& fore_color, const mpt_vec4f& back_color)
{
	glyph_ = glyph;
	brush_ = DEFAULT_BRUSH;

	if (fore_color != mpt_vec4f(0)) {
		color_ = fore_color;
		front_palette_color_ = -1;
	}

	if (back_color != mpt_vec4f(0)) {
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

void DocumentGlyph::SetBrush(short brush, short glyph, const mpt_vec4f& fore_color /*= mpt_vec4f(0)*/, const mpt_vec4f& back_color /*= mpt_vec4f(0)*/)
{
	glyph_ = glyph;
	brush_ = brush;

	if (fore_color != mpt_vec4f(0)) {
		color_ = fore_color;
		front_palette_color_ = -1;
	}

	if (back_color != mpt_vec4f(0)) {
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
	color_ = mpt_vec4f(DEFAULT_GLYPH);
	background_color_ = mpt_vec4f(0);
}

//
// Included: ../../Maptographer/src/Brush.cpp

//
// Included: ../../Maptographer/src/DocumentLayer.cpp
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
void DocumentLayer<T>::Resize(const mpt_vec2i& new_size_)
{
	if (new_size_ != size_ || (data_.size() != new_size_.x*new_size_.y)) {
		auto backup = data_;
		auto old_size_ = size_;

		ChangeArraySize((unsigned)(new_size_.x*new_size_.y));

		size_ = new_size_;

		for (int y = 0; y < old_size_.y; y++) {
			for (int x = 0; x < old_size_.x; x++) {
				int index = x + y * (int)old_size_.x;
				if (index >= (int)backup.size())
					continue;

				auto* old_tile = &backup[index];
				auto* new_tile = GetAt(mpt_vec2i(x, y));

				if (new_tile && old_tile )
					*new_tile = *old_tile;
			}
		}
	}
}

template<class T>
T * DocumentLayer<T>::GetAt(const mpt_vec2i & pt)
{
	return GetAt(pt.x, pt.y);
}

template<class T>
T* DocumentLayer<T>::GetAt(int x, int y)
{
	if (x >= 0 && x < (int)size_.x && y >= 0 && y < (int)size_.y)
		return &data_[x + y * (int)size_.x];

	return nullptr;
}

template<class T>
T * DocumentLayer<T>::GetAt(int n)
{
	if (n >= 0 && n < (int)(size_.x*size_.y))
		return &data_[n];
	return nullptr;
}

template<class T>
T * DocumentLayer<T>::GetAt(const mpt_vec2i & pt) const
{
	return GetAt(pt.x, pt.y);
}

template<class T>
T* DocumentLayer<T>::GetAt(int x, int y) const
{
	if (x >= 0 && x < (int)size_.x && y >= 0 && y < (int)size_.y)
		return const_cast<T*>(&data_[x + y * (int)size_.x]);

	return nullptr;
}

template<class T>
T * DocumentLayer<T>::GetAt(int n) const
{
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
template class DocumentLayer<DocumentGlyph>;	// 

//
// Included: ../../Maptographer/src/DocumentRegion.cpp
DocumentRegion::DocumentRegion()
{
}


//
// Included: ../../Maptographer/src/BaseDocument.cpp
// desc: clears document
template<class T>
void BaseDocument<T>::Clear() {
	layer_.Clear();
	draw_offset_ = mpt_vec2i(-1);

}
template<class T>
bool BaseDocument<T>::Resize(const mpt_vec2i& new_size_)
{
	if (new_size_.x > 0 && new_size_.y > 0) {
		layer_.Call(&DocumentLayer<T>::Resize, new_size_);


		size_ = new_size_;

		return true;
	}

	return false;
}
template class BaseDocument<DocumentGlyph>; // 

//
// Included: ../../Maptographer/src/Document.cpp
template <class T>
BrushPalette* Document<T>::CurrentBrushPalette()
{
	return brush_.Get(current_brush_palette_);
}

template<class T>
void Document<T>::SetKey(const binary_vector& key)
{
	key_ = key;
}

template<class T>
void Document<T>::RefreshBrushes(const BrushPalette* brushes) {

	if (brushes == nullptr) {

		if( auto* palette = CurrentBrushPalette()) {
			palette->Call(&Brush::RefreshSelf);
			palette->Call(&Brush::ForceRefresh);
			
			RefreshBrushes(palette);
			elements_.Call(&DocumentElement::RefreshBrushes, palette);
		}
	}
	else {
		BaseDocument<T>::layer_.Call(&DocumentLayer<T>::UpdateBrushes, brushes);
	}
}

template<class T>
void Document<T>::RebuildBrushes() {

	for (auto i = 0; i < brush_.Size(); ++i) {
		auto* brush_set = brush_.Get(i);
		for (auto u = 0; u < brush_set->Size(); u++) {
			auto* brush = brush_set->Get(u);
			brush->brush_ = (MAX_BRUSH_ENTRIES_PER_SET * i) + u;
		}
	}
}


template<class T>
LoadState Document<T>::Load(const mpt_string & path)
{
	Clear();
	BinaryIn file(path.c_str(), key_);

	if (file.is_open() == false)
		return LoadState::CantOpenFailed;

	auto result = file.template load(*this);

	if( result != LoadState::LoadOk ) {
	    file.close();
        return result;
    }

	file.close();
	RefreshBrushes();
	return LoadState::LoadOk;
} 
template<class T>
void Document<T>::Clear()
{
	BaseDocument<T>::Clear();
	elements_.Clear();
}
template class Document<DocumentGlyph>;	// 
