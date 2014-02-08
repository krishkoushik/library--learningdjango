from django.db import models
from django import forms


class Author(models.Model):
	name = models.CharField(max_length=200)
	
	def __unicode__(self):
		return self.name


class Publisher(models.Model):
	name = models.CharField(max_length=200)
	
	def __unicode__(self):
		return self.name


class Genre(models.Model):
	name = models.CharField(max_length=200)

	def __unicode__(self):
		return self.name


class Book(models.Model):
	name = models.CharField(max_length=200)
	publisher = models.ForeignKey(Publisher)
	genre = models.ForeignKey(Genre) 
	author = models.ManyToManyField(Author)

	def __unicode__(self):
		return self.name

	def print_full_name(self,array):
		return self.name + array
		

class UploadFileForm(forms.Form):
    title = forms.CharField(max_length=50)
    file  = forms.FileField()

# Creat your models here.
