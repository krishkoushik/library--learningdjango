from django.contrib import admin
from libapp.models import Book, Publisher, Genre, Author

admin.site.register(Book)
admin.site.register(Genre)
admin.site.register(Publisher)
admin.site.register(Author)
