from django.db import models
from django import forms
from django.core.files import File
from django.contrib.auth.models import User


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
		

class CodeToCompile(models.Model):
	user = models.OneToOneField(User)
	fil_e = models.CharField(max_length=100)
	compileoutp = models.CharField(max_length=100)
	runtimeoutp = models.CharField(max_length=100)
	compilemessage = models.CharField(max_length = 100)
#def save(self):
#		print self.fil_e.name+"YOYOY"
#		self.fil_e.save()


class UploadFileForm(forms.Form):
	fil_e  = forms.FileField()

# Create your models here.
